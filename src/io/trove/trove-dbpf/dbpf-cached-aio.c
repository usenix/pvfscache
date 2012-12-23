/*
 * (C) 2012 Wei Shi
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <assert.h>
#include <errno.h>

#include "gossip.h"
#include "pvfs2-debug.h"
#include "trove.h"
#include "trove-internal.h"
#include "dbpf.h"
#include "dbpf-op-queue.h"
#include "dbpf-bstream.h"
#include "dbpf-attr-cache.h"
#include "pint-event.h"
#include "dbpf-open-cache.h"
#include "dbpf-sync.h"

#include "dbpf-alt-aio.h"

extern gen_mutex_t dbpf_attr_cache_mutex;

#define AIOCB_ARRAY_SZ 64

extern int TROVE_max_concurrent_io;//In trove.c, default is 16
static int s_dbpf_ios_in_progress = 0;
static dbpf_op_queue_p s_dbpf_io_ready_queue = NULL;
static gen_mutex_t s_dbpf_io_mutex = GEN_MUTEX_INITIALIZER;
static gen_mutex_t dbpf_update_size_lock = GEN_MUTEX_INITIALIZER;

static struct dbpf_aio_ops aio_ops;

static int issue_or_delay_io_operation(
    dbpf_queued_op_t *cur_op, struct aiocb **aiocb_ptr_array,
    int aiocb_inuse_count, struct sigevent *sig, int dec_first);
static void start_delayed_ops_if_any(int dec_first);

#ifdef __PVFS2_TROVE_AIO_THREADED__
static char *list_proc_state_strings[] __attribute__((unused)) = {
    "LIST_PROC_INITIALIZED",
    "LIST_PROC_INPROGRESS ",
    "LIST_PROC_ALLCONVERTED",
    "LIST_PROC_ALLPOSTED",
};
#endif

#ifndef __PVFS2_TROVE_AIO_THREADED__
static int dbpf_bstream_rw_list_op_svc(struct dbpf_op *op_p);
#endif
static int dbpf_bstream_flush_op_svc(struct dbpf_op *op_p);

//#ifdef __PVFS2_TROVE_AIO_THREADED__
#include "dbpf-thread.h"
#include "pvfs2-internal.h"

extern pthread_cond_t dbpf_op_completed_cond;
extern dbpf_op_queue_p dbpf_completion_queue_array[TROVE_MAX_CONTEXTS];
extern gen_mutex_t dbpf_completion_queue_array_mutex[TROVE_MAX_CONTEXTS];





/* dbpf_bstream_rw_list()
 *
 * Handles queueing of both read and write list operations
 *
 * opcode parameter should be LIO_READ or LIO_WRITE
 */
inline int dbpf_bstream_rw_list(TROVE_coll_id coll_id,
                                TROVE_handle handle,
                                char **mem_offset_array,
                                TROVE_size *mem_size_array,
                                int mem_count,
                                TROVE_offset *stream_offset_array,
                                TROVE_size *stream_size_array,
                                int stream_count,
                                TROVE_size *out_size_p,
                                TROVE_ds_flags flags,
                                TROVE_vtag_s *vtag,
                                void *user_ptr,
                                TROVE_context_id context_id,
                                TROVE_op_id *out_op_id_p,
                                int opcode,
                                struct dbpf_aio_ops * aio_ops,
                                PVFS_hint  hints)
{
    gossip_debug(GOSSIP_WORMUP_DEBUG, "Here we enter into dbpf_bstream_rw_list\n");
    int ret = -TROVE_EINVAL;
    dbpf_queued_op_t *q_op_p = NULL;
    struct dbpf_collection *coll_p = NULL;
    enum dbpf_op_type tmp_type;
    PINT_event_type event_type;
    int i;
    PVFS_size count_mem;
//defined in null-aio, alt-aio; not in dbpf and directio
#ifdef __PVFS2_TROVE_AIO_THREADED__
    struct dbpf_op *op_p = NULL;
    int aiocb_inuse_count = 0;
    struct aiocb *aiocb_p = NULL, *aiocb_ptr_array[AIOCB_ARRAY_SZ] = {0};
#endif

    coll_p = dbpf_collection_find_registered(coll_id);
    if (coll_p == NULL)
    {
        return -TROVE_EINVAL;
    }

    q_op_p = dbpf_queued_op_alloc();
    if (q_op_p == NULL)
    {
        return -TROVE_ENOMEM;
    }

    if (opcode == LIO_READ)
    {
        tmp_type = BSTREAM_READ_LIST;
        event_type = trove_dbpf_read_event_id;
    }
    else
    {
        tmp_type = BSTREAM_WRITE_LIST;
        event_type = trove_dbpf_write_event_id;
    }

    /* initialize all the common members */
    dbpf_queued_op_init(q_op_p,
                        tmp_type,
                        handle,
                        coll_p,
#ifdef __PVFS2_TROVE_AIO_THREADED__
                        NULL,
#else
                        dbpf_bstream_rw_list_op_svc,
#endif
                        user_ptr,
                        flags,
                        context_id);

    if(PINT_EVENT_ENABLED)
    {
        count_mem = 0;
        for(i = 0; i < mem_count; ++i)
        {
            count_mem += mem_size_array[i];
        }
    }

    q_op_p->event_type = event_type;

    PINT_EVENT_START(event_type, dbpf_pid, NULL, &q_op_p->event_id,
                     PINT_HINT_GET_CLIENT_ID(hints),
                     PINT_HINT_GET_REQUEST_ID(hints),
                     PINT_HINT_GET_RANK(hints),
                     PINT_HINT_GET_HANDLE(hints),
                     handle,
                     PINT_HINT_GET_OP_ID(hints),
                     count_mem);

    if(gossip_debug_enabled(GOSSIP_TROVE_DEBUG))
    {
        PVFS_size count_stream = 0;
        count_mem = 0;
        gossip_debug(GOSSIP_TROVE_DEBUG,
                     "dbpf_bstream_rw_list: mem_count: %d, stream_count: %d\n",
                     mem_count,
                     stream_count);
        for(i = 0; i < mem_count; ++i)
        {
            gossip_debug(
                GOSSIP_TROVE_DEBUG,
                "dbpf_bstream_rw_list: mem_offset: %p, mem_size: %Ld\n",
                mem_offset_array[i], lld(mem_size_array[i]));
            count_mem += mem_size_array[i];
        }

        for(i = 0; i < stream_count; ++i)
        {
            gossip_debug(GOSSIP_TROVE_DEBUG,
                         "dbpf_bstream_rw_list: "
                         "stream_offset: %Ld, stream_size: %Ld\n",
                         lld(stream_offset_array[i]), lld(stream_size_array[i]));
            count_stream += stream_size_array[i];
        }

        if(count_mem != count_stream)
        {
            gossip_debug(GOSSIP_TROVE_DEBUG,
                         "dbpf_bstream_rw_list: "
                         "mem_count: %Ld != stream_count: %Ld\n",
                         lld(count_mem), lld(count_stream));
        }
    }

    /* initialize op-specific members */
    q_op_p->op.u.b_rw_list.fd = -1;
    q_op_p->op.u.b_rw_list.opcode = opcode;
    q_op_p->op.u.b_rw_list.mem_array_count = mem_count;
    q_op_p->op.u.b_rw_list.mem_offset_array = mem_offset_array;
    q_op_p->op.u.b_rw_list.mem_size_array = mem_size_array;
    q_op_p->op.u.b_rw_list.stream_array_count = stream_count;
    q_op_p->op.u.b_rw_list.stream_offset_array = stream_offset_array;
    q_op_p->op.u.b_rw_list.stream_size_array = stream_size_array;
    q_op_p->op.hints = hints;
    q_op_p->op.u.b_rw_list.aio_ops = aio_ops;

    /* initialize the out size to 0 */
    *out_size_p = 0;
    q_op_p->op.u.b_rw_list.out_size_p = out_size_p;
    q_op_p->op.u.b_rw_list.aiocb_array_count = 0;
    q_op_p->op.u.b_rw_list.aiocb_array = NULL;
#ifndef __PVFS2_TROVE_AIO_THREADED__
    q_op_p->op.u.b_rw_list.queued_op_ptr = (void *)q_op_p;
#endif

    /* initialize list processing state (more op-specific members) */
    q_op_p->op.u.b_rw_list.lio_state.mem_ct = 0;
    q_op_p->op.u.b_rw_list.lio_state.stream_ct = 0;
    q_op_p->op.u.b_rw_list.lio_state.cur_mem_size = mem_size_array[0];
    q_op_p->op.u.b_rw_list.lio_state.cur_mem_off = mem_offset_array[0];
    q_op_p->op.u.b_rw_list.lio_state.cur_stream_size =
        stream_size_array[0];
    q_op_p->op.u.b_rw_list.lio_state.cur_stream_off =
        stream_offset_array[0];

    q_op_p->op.u.b_rw_list.list_proc_state = LIST_PROC_INITIALIZED;

    ret = dbpf_open_cache_get(
        coll_id, handle,
        (opcode == LIO_WRITE) ? DBPF_FD_BUFFERED_WRITE : DBPF_FD_BUFFERED_READ,
        &q_op_p->op.u.b_rw_list.open_ref);
    if (ret < 0)
    {
        dbpf_queued_op_free(q_op_p);
        gossip_ldebug(GOSSIP_TROVE_DEBUG,
                      "warning: useless error value: %d\n", ret);
        return ret;
    }
    q_op_p->op.u.b_rw_list.fd = q_op_p->op.u.b_rw_list.open_ref.fd;

    /*
      if we're doing an i/o write, remove the cached attribute for
      this handle if it's present
    */
    if (opcode == LIO_WRITE)
    {
        TROVE_object_ref ref = {handle, coll_id};
        gen_mutex_lock(&dbpf_attr_cache_mutex);
        dbpf_attr_cache_remove(ref);
        gen_mutex_unlock(&dbpf_attr_cache_mutex);
    }

#ifndef __PVFS2_TROVE_AIO_THREADED__

    *out_op_id_p = dbpf_queued_op_queue(q_op_p);

#else
    op_p = &q_op_p->op;

    /*
      instead of queueing the op like most other trove operations,
      we're going to issue the system aio calls here to begin being
      serviced immediately.  We'll check progress in the
      aio_progress_notification callback method; this array is freed
      in dbpf-op.c:dbpf_queued_op_free
    */
    aiocb_p = (struct aiocb *)malloc(
        (AIOCB_ARRAY_SZ * sizeof(struct aiocb)));
    if (aiocb_p == NULL)
    {
        dbpf_open_cache_put(&q_op_p->op.u.b_rw_list.open_ref);
        return -TROVE_ENOMEM;
    }

    memset(aiocb_p, 0, (AIOCB_ARRAY_SZ * sizeof(struct aiocb)));
    for(i = 0; i < AIOCB_ARRAY_SZ; i++)
    {
        aiocb_p[i].aio_lio_opcode = LIO_NOP;
        aiocb_p[i].aio_sigevent.sigev_notify = SIGEV_NONE;
    }

    op_p->u.b_rw_list.aiocb_array_count = AIOCB_ARRAY_SZ;
    op_p->u.b_rw_list.aiocb_array = aiocb_p;
    op_p->u.b_rw_list.list_proc_state = LIST_PROC_INPROGRESS;

    /* convert listio arguments into aiocb structures */
    aiocb_inuse_count = op_p->u.b_rw_list.aiocb_array_count;
    ret = dbpf_bstream_listio_convert(
        op_p->u.b_rw_list.fd,
        op_p->u.b_rw_list.opcode,
        op_p->u.b_rw_list.mem_offset_array,
        op_p->u.b_rw_list.mem_size_array,
        op_p->u.b_rw_list.mem_array_count,
        op_p->u.b_rw_list.stream_offset_array,
        op_p->u.b_rw_list.stream_size_array,
        op_p->u.b_rw_list.stream_array_count,
        aiocb_p,
        &aiocb_inuse_count,
        &op_p->u.b_rw_list.lio_state);

    if (ret == 1)
    {
        op_p->u.b_rw_list.list_proc_state = LIST_PROC_ALLCONVERTED;
    }

    op_p->u.b_rw_list.sigev.sigev_notify = SIGEV_THREAD;
    op_p->u.b_rw_list.sigev.sigev_notify_attributes = NULL;
    op_p->u.b_rw_list.sigev.sigev_notify_function =
        aio_progress_notification;
    op_p->u.b_rw_list.sigev.sigev_value.sival_ptr = (void *)q_op_p;

    /* mark unused with LIO_NOPs */
    for(i = aiocb_inuse_count;
        i < op_p->u.b_rw_list.aiocb_array_count; i++)
    {
        aiocb_p[i].aio_lio_opcode = LIO_NOP;
    }

    for(i = 0; i < aiocb_inuse_count; i++)
    {
        aiocb_ptr_array[i] = &aiocb_p[i];
    }

    assert(q_op_p == op_p->u.b_rw_list.sigev.sigev_value.sival_ptr);

    if (op_p->u.b_rw_list.list_proc_state == LIST_PROC_ALLCONVERTED)
    {
        op_p->u.b_rw_list.list_proc_state = LIST_PROC_ALLPOSTED;
    }

    gen_mutex_lock(&q_op_p->mutex);
    q_op_p->op.state = OP_IN_SERVICE;
    gen_mutex_unlock(&q_op_p->mutex);

    id_gen_fast_register(&q_op_p->op.id, q_op_p);
    *out_op_id_p = q_op_p->op.id;

    ret = issue_or_delay_io_operation(
        q_op_p, aiocb_ptr_array, aiocb_inuse_count,
        &op_p->u.b_rw_list.sigev, 0);

    if (ret)
    {
        return ret;
    }
#endif
    return 0;
}

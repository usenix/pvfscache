/* WARNING: THIS FILE IS AUTOMATICALLY GENERATED FROM A .SM FILE.
 * Changes made here will certainly be overwritten.
 */

/* 
 * (C) 2001 Clemson University and The University of Chicago 
 *
 * See COPYING in top-level directory.
 */

#include <stdlib.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <string.h>
#include <assert.h>

#include "server-config.h"
#include "pvfs2-server.h"
#include "pvfs2-attr.h"
#include "pvfs2-internal.h"
#include "trove.h"

enum
{
    STATE_ENOTDIR = 7
};

static struct PINT_state_s ST_prelude;
static struct PINT_pjmp_tbl_s ST_prelude_pjtbl[];
static struct PINT_tran_tbl_s ST_prelude_trtbl[];

static PINT_sm_action readdir_verify_directory_metadata(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_verify_directory_metadata;
static struct PINT_pjmp_tbl_s ST_verify_directory_metadata_pjtbl[];
static struct PINT_tran_tbl_s ST_verify_directory_metadata_trtbl[];

static PINT_sm_action readdir_read_dirdata_handle(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_read_dirdata_handle;
static struct PINT_pjmp_tbl_s ST_read_dirdata_handle_pjtbl[];
static struct PINT_tran_tbl_s ST_read_dirdata_handle_trtbl[];

static PINT_sm_action readdir_iterate_on_entries(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_iterate_on_entries;
static struct PINT_pjmp_tbl_s ST_iterate_on_entries_pjtbl[];
static struct PINT_tran_tbl_s ST_iterate_on_entries_trtbl[];

static PINT_sm_action readdir_setup_resp(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_setup_resp;
static struct PINT_pjmp_tbl_s ST_setup_resp_pjtbl[];
static struct PINT_tran_tbl_s ST_setup_resp_trtbl[];
static struct PINT_state_s ST_final_response;
static struct PINT_pjmp_tbl_s ST_final_response_pjtbl[];
static struct PINT_tran_tbl_s ST_final_response_trtbl[];

static PINT_sm_action readdir_cleanup(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_cleanup;
static struct PINT_pjmp_tbl_s ST_cleanup_pjtbl[];
static struct PINT_tran_tbl_s ST_cleanup_trtbl[];

struct PINT_state_machine_s pvfs2_readdir_sm = {
	.name = "pvfs2_readdir_sm",
	.first_state = &ST_prelude
};

static struct PINT_state_s ST_prelude = {
	 .state_name = "prelude" ,
	 .parent_machine = &pvfs2_readdir_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_prelude_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_prelude_trtbl 
};

static struct PINT_tran_tbl_s ST_prelude_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_verify_directory_metadata },
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_verify_directory_metadata = {
	 .state_name = "verify_directory_metadata" ,
	 .parent_machine = &pvfs2_readdir_sm ,
	 .flag = SM_RUN ,
	 .action.func = readdir_verify_directory_metadata ,
	 .pjtbl = NULL ,
	 .trtbl = ST_verify_directory_metadata_trtbl 
};

static struct PINT_tran_tbl_s ST_verify_directory_metadata_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_read_dirdata_handle },
	{ .return_value = -1 ,
	 .next_state = &ST_setup_resp }
};

static struct PINT_state_s ST_read_dirdata_handle = {
	 .state_name = "read_dirdata_handle" ,
	 .parent_machine = &pvfs2_readdir_sm ,
	 .flag = SM_RUN ,
	 .action.func = readdir_read_dirdata_handle ,
	 .pjtbl = NULL ,
	 .trtbl = ST_read_dirdata_handle_trtbl 
};

static struct PINT_tran_tbl_s ST_read_dirdata_handle_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_iterate_on_entries },
	{ .return_value = -1 ,
	 .next_state = &ST_setup_resp }
};

static struct PINT_state_s ST_iterate_on_entries = {
	 .state_name = "iterate_on_entries" ,
	 .parent_machine = &pvfs2_readdir_sm ,
	 .flag = SM_RUN ,
	 .action.func = readdir_iterate_on_entries ,
	 .pjtbl = NULL ,
	 .trtbl = ST_iterate_on_entries_trtbl 
};

static struct PINT_tran_tbl_s ST_iterate_on_entries_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_setup_resp }
};

static struct PINT_state_s ST_setup_resp = {
	 .state_name = "setup_resp" ,
	 .parent_machine = &pvfs2_readdir_sm ,
	 .flag = SM_RUN ,
	 .action.func = readdir_setup_resp ,
	 .pjtbl = NULL ,
	 .trtbl = ST_setup_resp_trtbl 
};

static struct PINT_tran_tbl_s ST_setup_resp_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_final_response = {
	 .state_name = "final_response" ,
	 .parent_machine = &pvfs2_readdir_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_final_response_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_final_response_trtbl 
};

static struct PINT_tran_tbl_s ST_final_response_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_cleanup }
};

static struct PINT_state_s ST_cleanup = {
	 .state_name = "cleanup" ,
	 .parent_machine = &pvfs2_readdir_sm ,
	 .flag = SM_RUN ,
	 .action.func = readdir_cleanup ,
	 .pjtbl = NULL ,
	 .trtbl = ST_cleanup_trtbl 
};

static struct PINT_tran_tbl_s ST_cleanup_trtbl[] = {
	{ .return_value = -1 ,

	 .flag = SM_TERM }
};

# 75 "src/server/readdir.sm"


static PINT_sm_action readdir_verify_directory_metadata(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    PVFS_object_attr *attr = &s_op->attr;

    js_p->error_code = 0;

    gossip_debug(GOSSIP_READDIR_DEBUG, " - attrs: owner=%d, group=%d, "
                 "perms=%d\n\ttype=%d, mtime=%llu\n", attr->owner,
                 attr->group, attr->perms, attr->objtype,
                 llu(attr->mtime));

    if (attr->objtype != PVFS_TYPE_DIRECTORY)
    {
	gossip_debug(GOSSIP_READDIR_DEBUG, "  object is not a directory; "
                     "halting readdir and sending response\n");

	js_p->error_code = STATE_ENOTDIR;
    }

    s_op->u.readdir.directory_version = (uint64_t)attr->mtime;
    return SM_ACTION_COMPLETE;
}

static PINT_sm_action readdir_read_dirdata_handle(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret = -PVFS_EINVAL;
    job_id_t i;

    s_op->key.buffer = Trove_Common_Keys[DIR_ENT_KEY].key;
    s_op->key.buffer_sz = Trove_Common_Keys[DIR_ENT_KEY].size;

    s_op->val.buffer = &s_op->u.readdir.dirent_handle;
    s_op->val.buffer_sz = sizeof(PVFS_handle);

    gossip_debug(
        GOSSIP_READDIR_DEBUG, " - reading metadata: [%llu,%d], "
        "key=%s\n\t(sz=%d) val_buf=%p (sz=%d)\n",
        llu(s_op->req->u.readdir.handle), s_op->req->u.readdir.fs_id,
        (char *)s_op->key.buffer, s_op->key.buffer_sz,
        s_op->val.buffer, s_op->val.buffer_sz);

    ret = job_trove_keyval_read(
        s_op->req->u.readdir.fs_id, s_op->req->u.readdir.handle,
        &s_op->key, &s_op->val, 
        0, 
        NULL, smcb, 0, js_p, &i,
        server_job_context, s_op->req->hints);

    return ret;
}

static PINT_sm_action readdir_iterate_on_entries(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret = -PVFS_EINVAL;
    int j = 0, memory_size = 0, kv_array_size = 0;
    char *memory_buffer = NULL;
    job_id_t j_id;

    /*
      if a client issues a readdir but asks for no entries, we can
      skip doing anything here
    */
    if (s_op->req->u.readdir.dirent_count == 0)
    {
	js_p->error_code = 0;
        return SM_ACTION_COMPLETE;
    }

    if (s_op->req->u.readdir.dirent_count > PVFS_REQ_LIMIT_DIRENT_COUNT)
    {
        js_p->error_code = -PVFS_EINVAL;
        return SM_ACTION_COMPLETE;
    }

    /*
      calculate total memory needed:
      - 2 * dirent_count keyval structures to pass to iterate function
      - dirent_count dirent structures to hold the results
    */
    kv_array_size = (s_op->req->u.readdir.dirent_count *
                     sizeof(PVFS_ds_keyval));

    memory_size = (2 * kv_array_size +
                   s_op->req->u.readdir.dirent_count *
                   sizeof(PVFS_dirent));

    memory_buffer = malloc(memory_size);
    if (!memory_buffer)
    {
        js_p->error_code = -PVFS_ENOMEM;
        return SM_ACTION_COMPLETE;
    }

    /* set up all the pointers into the one big buffer */
    s_op->key_a = (PVFS_ds_keyval *)memory_buffer;
    memory_buffer += kv_array_size;

    s_op->val_a = (PVFS_ds_keyval *)memory_buffer;
    memory_buffer += kv_array_size;

    s_op->resp.u.readdir.dirent_array = (PVFS_dirent *)memory_buffer;

    for (j = 0; j < s_op->req->u.readdir.dirent_count; j++)
    {
	s_op->key_a[j].buffer =
            s_op->resp.u.readdir.dirent_array[j].d_name;
	s_op->key_a[j].buffer_sz = PVFS_NAME_MAX;
	s_op->val_a[j].buffer =
            &(s_op->resp.u.readdir.dirent_array[j].handle);
	s_op->val_a[j].buffer_sz = sizeof(PVFS_handle);
    }

    gossip_debug(
        GOSSIP_READDIR_DEBUG, " - iterating keyvals: [%llu,%d], "
        "\n\ttoken=%llu, count=%d\n",
        llu(s_op->u.readdir.dirent_handle), s_op->req->u.readdir.fs_id,
        llu(s_op->req->u.readdir.token),
        s_op->req->u.readdir.dirent_count);

    ret = job_trove_keyval_iterate(
        s_op->req->u.readdir.fs_id, s_op->u.readdir.dirent_handle,
        s_op->req->u.readdir.token, s_op->key_a, s_op->val_a,
        s_op->req->u.readdir.dirent_count, 
        0, 
        NULL, smcb, 0, js_p,
        &j_id, server_job_context, s_op->req->hints);

    return ret;
}

static PINT_sm_action readdir_setup_resp(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    if (js_p->error_code == STATE_ENOTDIR)
    {
	gossip_debug(GOSSIP_READDIR_DEBUG,
		     "  handle didn't refer to a directory\n");

	js_p->error_code = -PVFS_EINVAL;
	return SM_ACTION_COMPLETE;
    }
    else if(js_p->error_code != 0)
    {
        PVFS_perror_gossip("readdir_setup_resp failed: ", js_p->error_code);
        return SM_ACTION_COMPLETE;
    }

    s_op->resp.u.readdir.directory_version =
        s_op->u.readdir.directory_version;
    s_op->resp.u.readdir.dirent_count = js_p->count;

    /*
     * Although, this is not as important to get ls
     * to work through the VFS readdir, it is important
     * to fill this and send it back because the system
     * interface users could break because of this...
     */
    s_op->resp.u.readdir.token = js_p->position;
    js_p->error_code = 0;
    return SM_ACTION_COMPLETE;
}

static PINT_sm_action readdir_cleanup(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    if (s_op->key_a)
    {
        free(s_op->key_a);
        s_op->key_a = NULL;
        s_op->val_a = NULL;
        s_op->resp.u.readdir.dirent_array = NULL;
    }
    return(server_state_machine_complete(smcb));
}

PINT_GET_OBJECT_REF_DEFINE(readdir);

struct PINT_server_req_params pvfs2_readdir_params =
{
    .string_name = "readdir",
    .perm = PINT_SERVER_CHECK_READ,
    .get_object_ref = PINT_get_object_ref_readdir,
    .state_machine = &pvfs2_readdir_sm
};

/*
 * Local variables:
 *  mode: c
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ft=c ts=8 sts=4 sw=4 expandtab
 */

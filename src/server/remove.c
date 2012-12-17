/* WARNING: THIS FILE IS AUTOMATICALLY GENERATED FROM A .SM FILE.
 * Changes made here will certainly be overwritten.
 */

/* 
 * (C) 2001 Clemson University and The University of Chicago 
 *
 * See COPYING in top-level directory.
 */

#include <string.h>
#include <assert.h>

#include "server-config.h"
#include "pvfs2-storage.h"
#include "pvfs2-server.h"
#include "pvfs2-attr.h"
#include "gossip.h"
#include "pvfs2-internal.h"

/* Implementation notes
 *
 * This is a complicated machine.  It is capable of removing the three
 * externally visible PVFS2 object types: datafiles, metafiles, and
 * directories.
 *
 * For a datafile, the path through the state machine looks like:
 * 1) prelude
 * 2) check_object_type
 * 3) verify_object_metadata
 * 4) remove_dspace
 * 5) final_response
 * 6) cleanup
 *
 * For metafiles and symlinks, the path is:
 * 1) prelude
 * 2) check_object_type
 * 3) verify_object_metadata
 * 4) remove_dspace
 * 5) final_response
 * 6) cleanup
 *
 * For a directory that has (or at one time had) entries, the path is:
 * 1) prelude
 * 2) check_object_type
 * 3) verify_object_metadata
 * 4) try_to_read_dirdata_handle
 * 5) remove_dirdata_keyvals
 * 6) remove_dirdata_dspace
 * 7) remove_dspace
 * 8) final_response
 * 9) cleanup
 *
 * A directory that never had entries will skip step (5), as there
 * would be no dirdata dspace.
 */

#define PVFS_REMOVE_ITERATE_MAXCOUNT 100

enum
{
    STATE_TYPE_DIRECTORY = 1,
    STATE_TYPE_DATAFILE = 2,
};


static PINT_sm_action remove_check_object_type(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_check_object_type;
static struct PINT_pjmp_tbl_s ST_check_object_type_pjtbl[];
static struct PINT_tran_tbl_s ST_check_object_type_trtbl[];

static PINT_sm_action remove_verify_object_metadata(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_verify_object_metadata;
static struct PINT_pjmp_tbl_s ST_verify_object_metadata_pjtbl[];
static struct PINT_tran_tbl_s ST_verify_object_metadata_trtbl[];

static PINT_sm_action remove_remove_dspace(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_remove_dspace;
static struct PINT_pjmp_tbl_s ST_remove_dspace_pjtbl[];
static struct PINT_tran_tbl_s ST_remove_dspace_trtbl[];

static PINT_sm_action remove_read_dirdata_handle(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_read_dirdata_handle;
static struct PINT_pjmp_tbl_s ST_read_dirdata_handle_pjtbl[];
static struct PINT_tran_tbl_s ST_read_dirdata_handle_trtbl[];

static PINT_sm_action remove_get_dirent_count(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_getattr_of_dirdata_handle;
static struct PINT_pjmp_tbl_s ST_getattr_of_dirdata_handle_pjtbl[];
static struct PINT_tran_tbl_s ST_getattr_of_dirdata_handle_trtbl[];

static PINT_sm_action remove_check_dirdata_entries(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_check_dirdata_entries;
static struct PINT_pjmp_tbl_s ST_check_dirdata_entries_pjtbl[];
static struct PINT_tran_tbl_s ST_check_dirdata_entries_trtbl[];

static PINT_sm_action remove_remove_dirdata_dspace(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_remove_dirdata_dspace;
static struct PINT_pjmp_tbl_s ST_remove_dirdata_dspace_pjtbl[];
static struct PINT_tran_tbl_s ST_remove_dirdata_dspace_trtbl[];

struct PINT_state_machine_s pvfs2_remove_work_sm = {
	.name = "pvfs2_remove_work_sm",
	.first_state = &ST_check_object_type
};

static struct PINT_state_s ST_check_object_type = {
	 .state_name = "check_object_type" ,
	 .parent_machine = &pvfs2_remove_work_sm ,
	 .flag = SM_RUN ,
	 .action.func = remove_check_object_type ,
	 .pjtbl = NULL ,
	 .trtbl = ST_check_object_type_trtbl 
};

static struct PINT_tran_tbl_s ST_check_object_type_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_verify_object_metadata },
	{ .return_value = STATE_TYPE_DATAFILE ,
	 .next_state = &ST_remove_dspace },
	{ .return_value = -1 ,
	 .flag = SM_RETURN }
};

static struct PINT_state_s ST_verify_object_metadata = {
	 .state_name = "verify_object_metadata" ,
	 .parent_machine = &pvfs2_remove_work_sm ,
	 .flag = SM_RUN ,
	 .action.func = remove_verify_object_metadata ,
	 .pjtbl = NULL ,
	 .trtbl = ST_verify_object_metadata_trtbl 
};

static struct PINT_tran_tbl_s ST_verify_object_metadata_trtbl[] = {
	{ .return_value = STATE_TYPE_DIRECTORY ,
	 .next_state = &ST_read_dirdata_handle },
	{ .return_value = 0 ,
	 .next_state = &ST_remove_dspace },
	{ .return_value = -1 ,
	 .flag = SM_RETURN }
};

static struct PINT_state_s ST_remove_dspace = {
	 .state_name = "remove_dspace" ,
	 .parent_machine = &pvfs2_remove_work_sm ,
	 .flag = SM_RUN ,
	 .action.func = remove_remove_dspace ,
	 .pjtbl = NULL ,
	 .trtbl = ST_remove_dspace_trtbl 
};

static struct PINT_tran_tbl_s ST_remove_dspace_trtbl[] = {
	{ .return_value = -1 ,
	 .flag = SM_RETURN }
};

static struct PINT_state_s ST_read_dirdata_handle = {
	 .state_name = "read_dirdata_handle" ,
	 .parent_machine = &pvfs2_remove_work_sm ,
	 .flag = SM_RUN ,
	 .action.func = remove_read_dirdata_handle ,
	 .pjtbl = NULL ,
	 .trtbl = ST_read_dirdata_handle_trtbl 
};

static struct PINT_tran_tbl_s ST_read_dirdata_handle_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_getattr_of_dirdata_handle },
	{ .return_value = -1 ,
	 .next_state = &ST_remove_dspace }
};

static struct PINT_state_s ST_getattr_of_dirdata_handle = {
	 .state_name = "getattr_of_dirdata_handle" ,
	 .parent_machine = &pvfs2_remove_work_sm ,
	 .flag = SM_RUN ,
	 .action.func = remove_get_dirent_count ,
	 .pjtbl = NULL ,
	 .trtbl = ST_getattr_of_dirdata_handle_trtbl 
};

static struct PINT_tran_tbl_s ST_getattr_of_dirdata_handle_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_check_dirdata_entries }
};

static struct PINT_state_s ST_check_dirdata_entries = {
	 .state_name = "check_dirdata_entries" ,
	 .parent_machine = &pvfs2_remove_work_sm ,
	 .flag = SM_RUN ,
	 .action.func = remove_check_dirdata_entries ,
	 .pjtbl = NULL ,
	 .trtbl = ST_check_dirdata_entries_trtbl 
};

static struct PINT_tran_tbl_s ST_check_dirdata_entries_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_remove_dirdata_dspace },
	{ .return_value = -1 ,
	 .flag = SM_RETURN }
};

static struct PINT_state_s ST_remove_dirdata_dspace = {
	 .state_name = "remove_dirdata_dspace" ,
	 .parent_machine = &pvfs2_remove_work_sm ,
	 .flag = SM_RUN ,
	 .action.func = remove_remove_dirdata_dspace ,
	 .pjtbl = NULL ,
	 .trtbl = ST_remove_dirdata_dspace_trtbl 
};

static struct PINT_tran_tbl_s ST_remove_dirdata_dspace_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_remove_dspace },
	{ .return_value = -1 ,
	 .flag = SM_RETURN }
};

static struct PINT_state_s ST_prelude;
static struct PINT_pjmp_tbl_s ST_prelude_pjtbl[];
static struct PINT_tran_tbl_s ST_prelude_trtbl[];

static PINT_sm_action remove_setup_work(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_setup_work;
static struct PINT_pjmp_tbl_s ST_setup_work_pjtbl[];
static struct PINT_tran_tbl_s ST_setup_work_trtbl[];
static struct PINT_state_s ST_work;
static struct PINT_pjmp_tbl_s ST_work_pjtbl[];
static struct PINT_tran_tbl_s ST_work_trtbl[];
static struct PINT_state_s ST_final_response;
static struct PINT_pjmp_tbl_s ST_final_response_pjtbl[];
static struct PINT_tran_tbl_s ST_final_response_trtbl[];

static PINT_sm_action remove_cleanup(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_cleanup;
static struct PINT_pjmp_tbl_s ST_cleanup_pjtbl[];
static struct PINT_tran_tbl_s ST_cleanup_trtbl[];

struct PINT_state_machine_s pvfs2_remove_sm = {
	.name = "pvfs2_remove_sm",
	.first_state = &ST_prelude
};

static struct PINT_state_s ST_prelude = {
	 .state_name = "prelude" ,
	 .parent_machine = &pvfs2_remove_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_prelude_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_prelude_trtbl 
};

static struct PINT_tran_tbl_s ST_prelude_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_setup_work },
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_setup_work = {
	 .state_name = "setup_work" ,
	 .parent_machine = &pvfs2_remove_sm ,
	 .flag = SM_RUN ,
	 .action.func = remove_setup_work ,
	 .pjtbl = NULL ,
	 .trtbl = ST_setup_work_trtbl 
};

static struct PINT_tran_tbl_s ST_setup_work_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_work }
};

static struct PINT_state_s ST_work = {
	 .state_name = "work" ,
	 .parent_machine = &pvfs2_remove_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_remove_work_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_work_trtbl 
};

static struct PINT_tran_tbl_s ST_work_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_final_response = {
	 .state_name = "final_response" ,
	 .parent_machine = &pvfs2_remove_sm ,
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
	 .parent_machine = &pvfs2_remove_sm ,
	 .flag = SM_RUN ,
	 .action.func = remove_cleanup ,
	 .pjtbl = NULL ,
	 .trtbl = ST_cleanup_trtbl 
};

static struct PINT_tran_tbl_s ST_cleanup_trtbl[] = {
	{ .return_value = -1 ,

	 .flag = SM_TERM }
};

# 152 "src/server/remove.sm"


/*
 * Function: remove_check_object_type
 *
 * decides whether to follow the path for datafile removal or
 * non-datafile removal, based on whether we were able to successfully
 * retrive object attributes earlier
 *
 */
static PINT_sm_action remove_check_object_type(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    if(s_op->attr.objtype == PVFS_TYPE_DATAFILE)
    {
	/* it must be a datafile, transition to try reading datafile 
	 * attribs with dspace_getattr()
	 */
	js_p->error_code = STATE_TYPE_DATAFILE;
	return SM_ACTION_COMPLETE;
    }

    js_p->error_code = 0;
    return SM_ACTION_COMPLETE;
}

/*
 * Function: remove_verify_object_metadata
 *
 * Verifies that the user has permission to access the file (not yet
 * implemented).  Also directs removal of dirdata dspace in the case
 * of a directory.
 *
 * This state has changed some over time; it might need a new name.
 *
 * Note: errors from the previous state are redirected elsewhere, so
 * we know that we have metadata if we make it here.
 */
static PINT_sm_action remove_verify_object_metadata(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    PVFS_object_attr *a_p;

    a_p = &s_op->attr;

    js_p->error_code = 0;
    
    gossip_debug(GOSSIP_SERVER_DEBUG,
		 "  attrs read from keyval = (owner = %d, "
                 "group = %d, perms = %o, type = %d)\n",
		 a_p->owner,
		 a_p->group,
		 a_p->perms,
		 a_p->objtype);

    if (a_p->objtype == PVFS_TYPE_DIRECTORY)
    {
	js_p->error_code = STATE_TYPE_DIRECTORY;

	gossip_debug(
            GOSSIP_SERVER_DEBUG, "  type is directory; removing "
            "dirdata object before removing directory itself.\n");
    }

    return SM_ACTION_COMPLETE;
}

static PINT_sm_action remove_read_dirdata_handle(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret;
    job_id_t j_id;

    /* set up key and value structures for reading the dirdata handle */
    s_op->key.buffer = Trove_Common_Keys[DIR_ENT_KEY].key;
    s_op->key.buffer_sz = Trove_Common_Keys[DIR_ENT_KEY].size;

    s_op->val.buffer = &s_op->u.remove.dirdata_handle;
    s_op->val.buffer_sz = sizeof(PVFS_handle);

    gossip_debug(GOSSIP_SERVER_DEBUG,
		 "  trying to read dirdata handle (coll_id = %d, "
                 "handle = %llu, key = %s (%d), val_buf = %p (%d))\n",
		 s_op->u.remove.fs_id, llu(s_op->u.remove.handle),
		 (char *)s_op->key.buffer, s_op->key.buffer_sz,
		 s_op->val.buffer, s_op->val.buffer_sz);

    ret = job_trove_keyval_read(
        s_op->u.remove.fs_id, s_op->u.remove.handle,
        &s_op->key, &s_op->val,
        0,
        NULL, smcb, 0, js_p,
        &j_id, server_job_context, s_op->req->hints);

    return ret;
}

static PINT_sm_action remove_get_dirent_count(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret;
    job_id_t tmp_id;

    if (js_p->error_code)
    {
        return SM_ACTION_COMPLETE;
    }
    js_p->error_code = 0;


    ret = job_trove_keyval_iterate_keys(
        s_op->u.remove.fs_id,
        s_op->u.remove.dirdata_handle,
        PVFS_ITERATE_START,
        &s_op->u.remove.key,
        1,
        0,
        NULL,
        smcb,
        0,
        js_p,
        &tmp_id,
        server_job_context, s_op->req->hints);

    return ret;
}

static PINT_sm_action remove_check_dirdata_entries(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op __attribute__((unused)) =
        PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    if (js_p->error_code == 0)
    {
        if(js_p->count > 0)
        {
            gossip_debug(GOSSIP_SERVER_DEBUG, 
                         " detected non-empty "
                         "directory (fsid: %u, handle: %llu) "
                         "-- returning error\n",
                         s_op->u.remove.fs_id,
                         llu(s_op->u.remove.handle));
            js_p->error_code = -PVFS_ENOTEMPTY;
        }
    }
    else if(js_p->error_code == -TROVE_ENOENT)
    {
        /* no entries because count is 0 */
        js_p->error_code = 0;
    }

    /* setup position for next state (remove[_dirdata]_keyvals) */
    return SM_ACTION_COMPLETE;
}

/*
 * Function: remove_remove_dirdata_dspace
 *
 * Remove the dirdata dspace using the handle that we ready in the
 * read_dirdata_handle state.
 */
static PINT_sm_action remove_remove_dirdata_dspace(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret;
    job_id_t j_id;
    gossip_debug(GOSSIP_SERVER_DEBUG,
		 "  removing dirdata dspace %llu\n",
		 llu(s_op->u.remove.dirdata_handle));

    ret = job_trove_dspace_remove(
        s_op->u.remove.fs_id,
        s_op->u.remove.dirdata_handle,
        0,  /* don't sync here, we do a dspace remove on the dir handle next */
        smcb,
        0,
        js_p,
        &j_id,
        server_job_context, s_op->req->hints);

    return ret;
}

/*
 * Function: remove_remove_dspace
 *
 * Remove the dspace using the handle from the incoming request
 * (which was verified in previous states).
 */
static PINT_sm_action remove_remove_dspace(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret = -PVFS_EINVAL;
    job_id_t j_id;
    gossip_debug(GOSSIP_SERVER_DEBUG, "(%p) remove: removing dspace "
                 "object %llu,%d\n", s_op, llu(s_op->u.remove.handle),
                 s_op->u.remove.fs_id);

    ret = job_trove_dspace_remove(
        s_op->u.remove.fs_id, s_op->u.remove.handle,
        TROVE_SYNC,
        smcb, 0, js_p,
        &j_id, server_job_context, s_op->req->hints);

    return ret;
}

/*
 * Function: remove_cleanup
 *
 * Free all memory associated with this request and return 0, indicating
 * we're done processing.
 */
static PINT_sm_action remove_cleanup(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    return(server_state_machine_complete(smcb));
}

static PINT_sm_action remove_setup_work(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    s_op->u.remove.handle = s_op->req->u.remove.handle;
    s_op->u.remove.fs_id = s_op->req->u.remove.fs_id;
    js_p->error_code = 0;
    return SM_ACTION_COMPLETE;
}

PINT_GET_OBJECT_REF_DEFINE(remove);

struct PINT_server_req_params pvfs2_remove_params =
{
    .string_name = "remove",
    .get_object_ref = PINT_get_object_ref_remove,
    .perm = PINT_SERVER_CHECK_NONE,
    .access_type = PINT_server_req_modify,
    .sched_policy = PINT_SERVER_REQ_SCHEDULE,
    .state_machine = &pvfs2_remove_sm
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
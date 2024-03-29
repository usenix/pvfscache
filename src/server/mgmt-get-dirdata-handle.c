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

static struct PINT_state_s ST_prelude;
static struct PINT_pjmp_tbl_s ST_prelude_pjtbl[];
static struct PINT_tran_tbl_s ST_prelude_trtbl[];

static PINT_sm_action mgmt_get_dirdata_handle_from_parent(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_get_dirdata_handle_from_parent;
static struct PINT_pjmp_tbl_s ST_get_dirdata_handle_from_parent_pjtbl[];
static struct PINT_tran_tbl_s ST_get_dirdata_handle_from_parent_trtbl[];

static PINT_sm_action mgmt_get_dirdata_handle_setup_resp(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_get_dirdata_handle_setup_resp;
static struct PINT_pjmp_tbl_s ST_get_dirdata_handle_setup_resp_pjtbl[];
static struct PINT_tran_tbl_s ST_get_dirdata_handle_setup_resp_trtbl[];
static struct PINT_state_s ST_final_response;
static struct PINT_pjmp_tbl_s ST_final_response_pjtbl[];
static struct PINT_tran_tbl_s ST_final_response_trtbl[];

static PINT_sm_action mgmt_get_dirdata_handle_cleanup(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_cleanup;
static struct PINT_pjmp_tbl_s ST_cleanup_pjtbl[];
static struct PINT_tran_tbl_s ST_cleanup_trtbl[];

struct PINT_state_machine_s pvfs2_mgmt_get_dirdata_handle_sm = {
	.name = "pvfs2_mgmt_get_dirdata_handle_sm",
	.first_state = &ST_prelude
};

static struct PINT_state_s ST_prelude = {
	 .state_name = "prelude" ,
	 .parent_machine = &pvfs2_mgmt_get_dirdata_handle_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_prelude_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_prelude_trtbl 
};

static struct PINT_tran_tbl_s ST_prelude_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_get_dirdata_handle_from_parent },
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_get_dirdata_handle_from_parent = {
	 .state_name = "get_dirdata_handle_from_parent" ,
	 .parent_machine = &pvfs2_mgmt_get_dirdata_handle_sm ,
	 .flag = SM_RUN ,
	 .action.func = mgmt_get_dirdata_handle_from_parent ,
	 .pjtbl = NULL ,
	 .trtbl = ST_get_dirdata_handle_from_parent_trtbl 
};

static struct PINT_tran_tbl_s ST_get_dirdata_handle_from_parent_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_get_dirdata_handle_setup_resp },
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_get_dirdata_handle_setup_resp = {
	 .state_name = "get_dirdata_handle_setup_resp" ,
	 .parent_machine = &pvfs2_mgmt_get_dirdata_handle_sm ,
	 .flag = SM_RUN ,
	 .action.func = mgmt_get_dirdata_handle_setup_resp ,
	 .pjtbl = NULL ,
	 .trtbl = ST_get_dirdata_handle_setup_resp_trtbl 
};

static struct PINT_tran_tbl_s ST_get_dirdata_handle_setup_resp_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_final_response = {
	 .state_name = "final_response" ,
	 .parent_machine = &pvfs2_mgmt_get_dirdata_handle_sm ,
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
	 .parent_machine = &pvfs2_mgmt_get_dirdata_handle_sm ,
	 .flag = SM_RUN ,
	 .action.func = mgmt_get_dirdata_handle_cleanup ,
	 .pjtbl = NULL ,
	 .trtbl = ST_cleanup_trtbl 
};

static struct PINT_tran_tbl_s ST_cleanup_trtbl[] = {
	{ .return_value = -1 ,

	 .flag = SM_TERM }
};

# 54 "src/server/mgmt-get-dirdata-handle.sm"


static int mgmt_get_dirdata_handle_from_parent(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret = -PVFS_EINVAL;
    job_id_t i;

    s_op->key.buffer = Trove_Common_Keys[DIR_ENT_KEY].key;
    s_op->key.buffer_sz = Trove_Common_Keys[DIR_ENT_KEY].size;

    s_op->val.buffer = &s_op->u.mgmt_get_dirdata_handle.dirdata_handle;
    s_op->val.buffer_sz = sizeof(PVFS_handle);

    gossip_debug(
        GOSSIP_SERVER_DEBUG,
        "  reading dirdata handle (coll_id = %d, handle = %llu\n\t"
        "key = %s (%d), val_buf = %p (%d))\n",
        s_op->req->u.mgmt_get_dirdata_handle.fs_id,
        llu(s_op->req->u.mgmt_get_dirdata_handle.handle),
        (char *)s_op->key.buffer, s_op->key.buffer_sz,
        s_op->val.buffer, s_op->val.buffer_sz);

    ret = job_trove_keyval_read(
        s_op->req->u.mgmt_get_dirdata_handle.fs_id,
        s_op->req->u.mgmt_get_dirdata_handle.handle,
        &s_op->key, &s_op->val, 
        0, 
        NULL, smcb, 0, js_p, &i,
        server_job_context, s_op->req->hints);

    return ret;
}

static PINT_sm_action mgmt_get_dirdata_handle_setup_resp(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    if (js_p->error_code == 0)
    {
        s_op->resp.u.mgmt_get_dirdata_handle.handle =
            s_op->u.mgmt_get_dirdata_handle.dirdata_handle;

        gossip_debug(
            GOSSIP_SERVER_DEBUG, "  Found dirdata handle %llu of parent "
            "%llu\n", llu(s_op->u.mgmt_get_dirdata_handle.dirdata_handle),
            llu(s_op->req->u.mgmt_get_dirdata_handle.handle));
    }
    else
    {
        s_op->resp.u.mgmt_get_dirdata_handle.handle = PVFS_HANDLE_NULL;

        gossip_debug(GOSSIP_SERVER_DEBUG, "Failed to retrieve dirdata "
                     "handle from parent %llu\n",
                     llu(s_op->req->u.mgmt_get_dirdata_handle.handle));
    }
    return SM_ACTION_COMPLETE;
}

static PINT_sm_action mgmt_get_dirdata_handle_cleanup(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    return(server_state_machine_complete(smcb));
}

PINT_GET_OBJECT_REF_DEFINE(mgmt_get_dirdata_handle);

struct PINT_server_req_params pvfs2_mgmt_get_dirdata_handle_params =
{
    .string_name = "mgmt-get-dirdata-handle",
    .perm = PINT_SERVER_CHECK_NONE,
    .sched_policy = PINT_SERVER_REQ_SCHEDULE,
    .get_object_ref = PINT_get_object_ref_mgmt_get_dirdata_handle,
    .state_machine = &pvfs2_mgmt_get_dirdata_handle_sm
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

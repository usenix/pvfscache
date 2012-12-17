/* WARNING: THIS FILE IS AUTOMATICALLY GENERATED FROM A .SM FILE.
 * Changes made here will certainly be overwritten.
 */

/* 
 * (C) 2001 Clemson University and The University of Chicago 
 *
 * See COPYING in top-level directory.
 *
 */

/* pvfs2_list_attr_sm
 *
 * This state machine handles incoming server listattr operations for a list of handles.  These
 * are the operations sent by PVFS_sys_readdirplus().
 *
 */

#include <string.h>
#include <assert.h>

#include "server-config.h"
#include "pvfs2-server.h"
#include "pvfs2-attr.h"
#include "pvfs2-types.h"
#include "pvfs2-types-debug.h"
#include "pvfs2-util.h"
#include "pint-util.h"
#include "pvfs2-internal.h"

static struct PINT_state_s ST_prelude;
static struct PINT_pjmp_tbl_s ST_prelude_pjtbl[];
static struct PINT_tran_tbl_s ST_prelude_trtbl[];

static PINT_sm_action listattr_read_basic_attrs(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_read_basic_attrs;
static struct PINT_pjmp_tbl_s ST_read_basic_attrs_pjtbl[];
static struct PINT_tran_tbl_s ST_read_basic_attrs_trtbl[];

static PINT_sm_action listattr_setup_getattr(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_setup_getattr;
static struct PINT_pjmp_tbl_s ST_setup_getattr_pjtbl[];
static struct PINT_tran_tbl_s ST_setup_getattr_trtbl[];

static PINT_sm_action listattr_interpret_getattrs(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_interpret_getattrs;
static struct PINT_pjmp_tbl_s ST_interpret_getattrs_pjtbl[];
static struct PINT_tran_tbl_s ST_interpret_getattrs_trtbl[];
static struct PINT_state_s ST_final_response;
static struct PINT_pjmp_tbl_s ST_final_response_pjtbl[];
static struct PINT_tran_tbl_s ST_final_response_trtbl[];

static PINT_sm_action listattr_cleanup(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_cleanup;
static struct PINT_pjmp_tbl_s ST_cleanup_pjtbl[];
static struct PINT_tran_tbl_s ST_cleanup_trtbl[];

struct PINT_state_machine_s pvfs2_list_attr_sm = {
	.name = "pvfs2_list_attr_sm",
	.first_state = &ST_prelude
};

static struct PINT_state_s ST_prelude = {
	 .state_name = "prelude" ,
	 .parent_machine = &pvfs2_list_attr_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_prelude_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_prelude_trtbl 
};

static struct PINT_tran_tbl_s ST_prelude_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_read_basic_attrs },
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_read_basic_attrs = {
	 .state_name = "read_basic_attrs" ,
	 .parent_machine = &pvfs2_list_attr_sm ,
	 .flag = SM_RUN ,
	 .action.func = listattr_read_basic_attrs ,
	 .pjtbl = NULL ,
	 .trtbl = ST_read_basic_attrs_trtbl 
};

static struct PINT_tran_tbl_s ST_read_basic_attrs_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_setup_getattr },
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_setup_getattr = {
	 .state_name = "setup_getattr" ,
	 .parent_machine = &pvfs2_list_attr_sm ,
	 .flag = SM_PJMP ,
	 .action.func = &listattr_setup_getattr ,
	 .pjtbl = ST_setup_getattr_pjtbl ,
	 .trtbl = ST_setup_getattr_trtbl 
};

static struct PINT_pjmp_tbl_s ST_setup_getattr_pjtbl[] = {
	{ .return_value = 0 ,

	 .state_machine = &pvfs2_get_attr_work_sm }
};

static struct PINT_tran_tbl_s ST_setup_getattr_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_interpret_getattrs },
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_interpret_getattrs = {
	 .state_name = "interpret_getattrs" ,
	 .parent_machine = &pvfs2_list_attr_sm ,
	 .flag = SM_RUN ,
	 .action.func = listattr_interpret_getattrs ,
	 .pjtbl = NULL ,
	 .trtbl = ST_interpret_getattrs_trtbl 
};

static struct PINT_tran_tbl_s ST_interpret_getattrs_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_final_response = {
	 .state_name = "final_response" ,
	 .parent_machine = &pvfs2_list_attr_sm ,
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
	 .parent_machine = &pvfs2_list_attr_sm ,
	 .flag = SM_RUN ,
	 .action.func = listattr_cleanup ,
	 .pjtbl = NULL ,
	 .trtbl = ST_cleanup_trtbl 
};

static struct PINT_tran_tbl_s ST_cleanup_trtbl[] = {
	{ .return_value = -1 ,

	 .flag = SM_TERM }
};

# 75 "src/server/list-attr.sm"


static PINT_sm_action listattr_read_basic_attrs(
    struct PINT_smcb *smcb, job_status_s *js_p)
{    
    int ret;
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    job_id_t tmp_id;

    s_op->u.listattr.ds_attr_a = (PVFS_ds_attributes *) 
        calloc(s_op->req->u.listattr.nhandles * sizeof(PVFS_ds_attributes), 1);
    if (s_op->u.listattr.ds_attr_a == NULL) {
        js_p->error_code = -PVFS_ENOMEM;
        return SM_ACTION_COMPLETE;
    }

    s_op->u.listattr.errors = (PVFS_error *)
        calloc(s_op->req->u.listattr.nhandles * sizeof(PVFS_error), 1);
    if (s_op->u.listattr.errors == NULL) {
        js_p->error_code = -PVFS_ENOMEM;
        return SM_ACTION_COMPLETE;
    }
    s_op->u.listattr.attr_a = (PVFS_object_attr *)
        calloc(s_op->req->u.listattr.nhandles * sizeof(PVFS_object_attr), 1);
    if (s_op->u.listattr.attr_a == NULL) {
        js_p->error_code = -PVFS_ENOMEM;
        return SM_ACTION_COMPLETE;
    }

    js_p->error_code = 0;
    /* initiate retrieval of the attributes from the dspace */
    ret = job_trove_dspace_getattr_list(
            s_op->req->u.listattr.fs_id,
            s_op->req->u.listattr.nhandles,
            s_op->req->u.listattr.handles,
            smcb,
            s_op->u.listattr.errors,
            s_op->u.listattr.ds_attr_a,
            0,
            js_p,
            &tmp_id,
            server_job_context,
            s_op->req->hints);

    return ret;
}


static PINT_sm_action listattr_setup_getattr(
    struct PINT_smcb *smcb, job_status_s *js_p)
{    
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    struct PINT_server_op *getattr_op;
    int ret;
    int i;

    s_op->u.listattr.parallel_sms = 0;
    js_p->error_code = 0;

    for(i=0; i<s_op->req->u.listattr.nhandles; i++)
    {
        if(s_op->u.listattr.errors[i])
        {
            gossip_debug(GOSSIP_SERVER_DEBUG, 
                "listattr: dspace_getattr_list failed to get attrs for handle: %llu\n", 
                llu(s_op->req->u.listattr.handles[i]));
            /* skip nested machine for this handle */
            continue;
        }

        getattr_op = malloc(sizeof(*getattr_op));
        if(!getattr_op)
        {
            s_op->u.listattr.errors[i] = -PVFS_ENOMEM;
            gossip_debug(GOSSIP_SERVER_DEBUG, 
                "listattr: failed to setup nested sm for handle: %llu\n", 
                llu(s_op->req->u.listattr.handles[i]));
            continue;
        }
        memset(getattr_op, 0, sizeof(*getattr_op));

        /* TODO: need a way to explicitly set the right inputs to the
         * getattr nested sm.  This code block is very fragile.
         */

        /* need attrs that the prelude would have read normally */
        PVFS_ds_attr_to_object_attr(&s_op->u.listattr.ds_attr_a[i],
            &getattr_op->attr);
        getattr_op->attr.mask = PVFS_ATTR_COMMON_ALL;
        getattr_op->ds_attr = s_op->u.listattr.ds_attr_a[i];
        /* need a valid request structure for some generic features like access
         * logging 
         */
        getattr_op->req = s_op->req;
        /* need to fill in the input parameters to the getattr nested machine */
        getattr_op->u.getattr.fs_id = s_op->req->u.listattr.fs_id;
        getattr_op->u.getattr.handle = s_op->req->u.listattr.handles[i];
        getattr_op->u.getattr.attrmask = s_op->req->u.listattr.attrmask;

        ret = PINT_sm_push_frame(smcb, 0, getattr_op);
        if(ret < 0)
        {
            s_op->u.listattr.errors[i] = -PVFS_ENOMEM;
            gossip_debug(GOSSIP_SERVER_DEBUG, 
                "listattr: failed to setup nested sm for handle: %llu\n", 
                llu(s_op->req->u.listattr.handles[i]));
            continue;
        }

        s_op->u.listattr.parallel_sms++;
    }

    gossip_debug(GOSSIP_SERVER_DEBUG, 
        "listattr: set up %d parallel nested getattr machines.\n", 
        s_op->u.listattr.parallel_sms);

    if(s_op->u.listattr.parallel_sms > 0)
    {
        js_p->error_code = 0;
        return SM_ACTION_COMPLETE;
    }
    else
    {
        /* we didn't kick off any parallel machines.  Pick an error code and
         * move along...
         */
        js_p->error_code = s_op->u.listattr.errors[0];
        return SM_ACTION_COMPLETE;
    }
}

static PINT_sm_action listattr_interpret_getattrs(struct PINT_smcb *smcb, 
    job_status_s *js_p)
{    
    struct PINT_server_op *getattr_op;
    /* note: this gives us a pointer to the base frame (list_attr), 
     * _not_ the getattr frames that were previously pushed.
     */
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int task_id;
    int remaining;
    PVFS_error tmp_err;
    int i, j;

    assert(s_op);
    assert(s_op->op == PVFS_SERV_LISTATTR);

    gossip_debug(GOSSIP_SERVER_DEBUG, 
        "listattr: trying to interpret results from %d nested parallel getattr machines.\n", 
        s_op->u.listattr.parallel_sms);

    /* gather results */
    for(i=0; i<s_op->u.listattr.parallel_sms; i++)
    {
        getattr_op = PINT_sm_pop_frame(smcb, &task_id, &tmp_err, 
            &remaining);
        gossip_debug(GOSSIP_SERVER_DEBUG, "listattr: nested sm returned error code: %d\n", tmp_err);
        /* match it up with the correct array entry */
        for(j=0; j<s_op->req->u.listattr.nhandles; j++)
        {
            if(s_op->req->u.listattr.handles[j] == getattr_op->u.getattr.handle)
            {
                s_op->u.listattr.attr_a[j] = getattr_op->resp.u.getattr.attr;
                s_op->u.listattr.errors[j] = tmp_err;
                free(getattr_op);
                break;
            }
        }
    }

    /* if we reached this point, then we have a successful ack to send back;
     * set remaining response fields
     */
    s_op->resp.u.listattr.attr = s_op->u.listattr.attr_a;
    s_op->resp.u.listattr.error = s_op->u.listattr.errors;
    s_op->resp.u.listattr.nhandles = s_op->req->u.listattr.nhandles;

    js_p->error_code = 0;
    return SM_ACTION_COMPLETE;
}

static PINT_sm_action listattr_cleanup(struct PINT_smcb *smcb, job_status_s *js_p)
{
    int i;
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);

    if (s_op->u.listattr.attr_a)
    {
        for (i = 0; i < s_op->req->u.listattr.nhandles; i++) 
        {
            PINT_free_object_attr(&s_op->u.listattr.attr_a[i]);
        }
        free(s_op->u.listattr.attr_a);
    }
    if (s_op->u.listattr.ds_attr_a)
        free(s_op->u.listattr.ds_attr_a);
    if (s_op->u.listattr.errors)
        free(s_op->u.listattr.errors);

    return(server_state_machine_complete(smcb));
}

static inline int PINT_get_object_ref_listattr(
    struct PVFS_server_req *req, PVFS_fs_id *fs_id, PVFS_handle *handle)
{
    *fs_id = req->u.listattr.fs_id;
    *handle = PVFS_HANDLE_NULL;
    return 0;
};

struct PINT_server_req_params pvfs2_list_attr_params =
{
    .string_name = "list_attr",
    .perm = PINT_SERVER_CHECK_NONE,
    .access_type = PINT_server_req_readonly,
    .sched_policy = PINT_SERVER_REQ_SCHEDULE,
    .get_object_ref = PINT_get_object_ref_listattr,
    .state_machine = &pvfs2_list_attr_sm
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


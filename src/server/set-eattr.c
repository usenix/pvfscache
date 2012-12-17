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
#include "pvfs2-server.h"
#include "pvfs2-attr.h"
#include "pvfs2-internal.h"
#include "pvfs2-util.h"
#include "pint-util.h"
#include "pint-eattr.h"

static struct PINT_state_s ST_prelude;
static struct PINT_pjmp_tbl_s ST_prelude_pjtbl[];
static struct PINT_tran_tbl_s ST_prelude_trtbl[];

static PINT_sm_action seteattr_verify_eattribs(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_verify_eattrib;
static struct PINT_pjmp_tbl_s ST_verify_eattrib_pjtbl[];
static struct PINT_tran_tbl_s ST_verify_eattrib_trtbl[];

static PINT_sm_action seteattr_setobj_eattribs(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_setobj_eattrib;
static struct PINT_pjmp_tbl_s ST_setobj_eattrib_pjtbl[];
static struct PINT_tran_tbl_s ST_setobj_eattrib_trtbl[];
static struct PINT_state_s ST_final_response;
static struct PINT_pjmp_tbl_s ST_final_response_pjtbl[];
static struct PINT_tran_tbl_s ST_final_response_trtbl[];

static PINT_sm_action seteattr_cleanup(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_cleanup;
static struct PINT_pjmp_tbl_s ST_cleanup_pjtbl[];
static struct PINT_tran_tbl_s ST_cleanup_trtbl[];

struct PINT_state_machine_s pvfs2_set_eattr_sm = {
	.name = "pvfs2_set_eattr_sm",
	.first_state = &ST_prelude
};

static struct PINT_state_s ST_prelude = {
	 .state_name = "prelude" ,
	 .parent_machine = &pvfs2_set_eattr_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_prelude_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_prelude_trtbl 
};

static struct PINT_tran_tbl_s ST_prelude_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_verify_eattrib },
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_verify_eattrib = {
	 .state_name = "verify_eattrib" ,
	 .parent_machine = &pvfs2_set_eattr_sm ,
	 .flag = SM_RUN ,
	 .action.func = seteattr_verify_eattribs ,
	 .pjtbl = NULL ,
	 .trtbl = ST_verify_eattrib_trtbl 
};

static struct PINT_tran_tbl_s ST_verify_eattrib_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_setobj_eattrib },
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_setobj_eattrib = {
	 .state_name = "setobj_eattrib" ,
	 .parent_machine = &pvfs2_set_eattr_sm ,
	 .flag = SM_RUN ,
	 .action.func = seteattr_setobj_eattribs ,
	 .pjtbl = NULL ,
	 .trtbl = ST_setobj_eattrib_trtbl 
};

static struct PINT_tran_tbl_s ST_setobj_eattrib_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_final_response }
};

static struct PINT_state_s ST_final_response = {
	 .state_name = "final_response" ,
	 .parent_machine = &pvfs2_set_eattr_sm ,
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
	 .parent_machine = &pvfs2_set_eattr_sm ,
	 .flag = SM_RUN ,
	 .action.func = seteattr_cleanup ,
	 .pjtbl = NULL ,
	 .trtbl = ST_cleanup_trtbl 
};

static struct PINT_tran_tbl_s ST_cleanup_trtbl[] = {
	{ .return_value = -1 ,

	 .flag = SM_TERM }
};

# 55 "src/server/set-eattr.sm"


/*
 * This routine checks keys to confirm that the name spaces are valid.
 * It also prints debugging information.
 */
static int seteattr_verify_eattribs(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int i;
    PVFS_object_attr *a_p = NULL;
    a_p = &s_op->attr;

    js_p->error_code = 0;

    gossip_debug(GOSSIP_SETEATTR_DEBUG,
                 "  ext attr list write to handle %llu refers to a %s\n\t"
                 "[owner = %d, group = %d, perms = %o, type = %d]\n",
                 llu(s_op->req->u.seteattr.handle),
                 PINT_util_get_object_type(a_p->objtype),
                 a_p->owner, a_p->group, a_p->perms, a_p->objtype);

    /* iterate through the keys that are being written */
    for (i = 0; i < s_op->req->u.seteattr.nkey; i++)
    {
        js_p->error_code = PINT_eattr_namespace_verify(
            &s_op->req->u.seteattr.key[i],
            &s_op->req->u.seteattr.val[i]);
        if(!js_p->error_code)
        {
            return SM_ACTION_COMPLETE;
        }
    }

    return SM_ACTION_COMPLETE;
}

/*
 * This is where the actual extended attrib gets written.
 * Not much to this, its pretty straight-forward.
 * 
 * NOTE: we need to check the value of s_op->req->u.seteattr.flags to determine
 * the following:
 *
 * if (flags & PVFS_XATTR_CREATE)
 *  we need to make sure that the attribute does not exist. (pure create)
 * else if (flags & PVFS_XATTR_REPLACE)
 *  we need to make sure that the attribute does exist. (pure replace)
 * else it is default operation as before.
 * We translate this to an internal trove flag 
 *  (TROVE_NOOVERWRITE, TROVE_ONLYOVERWRITE)
 */
static int seteattr_setobj_eattribs(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_server_op *s_op = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret = 0;
    PVFS_ds_flags trove_flags = 0;
    job_id_t j_id;

    gossip_debug(
        GOSSIP_SETEATTR_DEBUG,
        "  writing keylist [%llu,%d,len %d flags %d]\n",
        llu(s_op->req->u.seteattr.handle),
        s_op->req->u.seteattr.fs_id,
        s_op->req->u.seteattr.nkey, s_op->req->u.seteattr.flags);

    if (s_op->req->u.seteattr.flags & PVFS_XATTR_CREATE)
    {
        trove_flags = TROVE_NOOVERWRITE;
    }
    else if (s_op->req->u.seteattr.flags & PVFS_XATTR_REPLACE)
    {
        trove_flags = TROVE_ONLYOVERWRITE;
    }
    trove_flags |= TROVE_SYNC;

    ret = job_trove_keyval_write_list(
        s_op->req->u.seteattr.fs_id,
        s_op->req->u.seteattr.handle,
        s_op->req->u.seteattr.key,
        s_op->req->u.seteattr.val,
        s_op->req->u.seteattr.nkey,
        trove_flags,
        NULL,
        smcb,
        0,
        js_p,
        &j_id,
        server_job_context, s_op->req->hints);

    return ret;
}

/*
 * Function: setattr_cleanup
 *
 * Params:   server_op *b, 
 *           job_status_s *js_p
 *
 * Returns:  int
 *
 * Synopsis: free memory and return
 *           
 */
static int seteattr_cleanup(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    /* do I need to free memory from decoding key and val?*/
    return(server_state_machine_complete(smcb));
}

PINT_GET_OBJECT_REF_DEFINE(seteattr);

struct PINT_server_req_params pvfs2_set_eattr_params =
{
    .string_name = "set_eattr",
    .perm = PINT_SERVER_CHECK_ATTR,
    .access_type = PINT_server_req_modify,
    .sched_policy = PINT_SERVER_REQ_SCHEDULE,
    .get_object_ref = PINT_get_object_ref_seteattr,
    .state_machine = &pvfs2_set_eattr_sm
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
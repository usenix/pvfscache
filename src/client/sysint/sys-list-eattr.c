/* WARNING: THIS FILE IS AUTOMATICALLY GENERATED FROM A .SM FILE.
 * Changes made here will certainly be overwritten.
 */

/* 
 * (C) 2003 Clemson University and The University of Chicago 
 *
 * See COPYING in top-level directory.
 */

#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "client-state-machine.h"
#include "pvfs2-debug.h"
#include "pvfs2-util.h"
#include "job.h"
#include "gossip.h"
#include "str-utils.h"
#include "pint-cached-config.h"
#include "PINT-reqproto-encode.h"

extern job_context_id pint_client_sm_context;

static int list_eattr_comp_fn(
    void *v_p,
    struct PVFS_server_resp *resp_p,
    int i);


static PINT_sm_action list_eattr_setup_msgpair(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_setup_msgpair;
static struct PINT_pjmp_tbl_s ST_setup_msgpair_pjtbl[];
static struct PINT_tran_tbl_s ST_setup_msgpair_trtbl[];
static struct PINT_state_s ST_xfer_msgpair;
static struct PINT_pjmp_tbl_s ST_xfer_msgpair_pjtbl[];
static struct PINT_tran_tbl_s ST_xfer_msgpair_trtbl[];

static PINT_sm_action list_eattr_cleanup(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_cleanup;
static struct PINT_pjmp_tbl_s ST_cleanup_pjtbl[];
static struct PINT_tran_tbl_s ST_cleanup_trtbl[];

struct PINT_state_machine_s pvfs2_client_list_eattr_sm = {
	.name = "pvfs2_client_list_eattr_sm",
	.first_state = &ST_setup_msgpair
};

static struct PINT_state_s ST_setup_msgpair = {
	 .state_name = "setup_msgpair" ,
	 .parent_machine = &pvfs2_client_list_eattr_sm ,
	 .flag = SM_RUN ,
	 .action.func = list_eattr_setup_msgpair ,
	 .pjtbl = NULL ,
	 .trtbl = ST_setup_msgpair_trtbl 
};

static struct PINT_tran_tbl_s ST_setup_msgpair_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_xfer_msgpair },
	{ .return_value = -1 ,
	 .next_state = &ST_cleanup }
};

static struct PINT_state_s ST_xfer_msgpair = {
	 .state_name = "xfer_msgpair" ,
	 .parent_machine = &pvfs2_client_list_eattr_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_msgpairarray_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_xfer_msgpair_trtbl 
};

static struct PINT_tran_tbl_s ST_xfer_msgpair_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_cleanup }
};

static struct PINT_state_s ST_cleanup = {
	 .state_name = "cleanup" ,
	 .parent_machine = &pvfs2_client_list_eattr_sm ,
	 .flag = SM_RUN ,
	 .action.func = list_eattr_cleanup ,
	 .pjtbl = NULL ,
	 .trtbl = ST_cleanup_trtbl 
};

static struct PINT_tran_tbl_s ST_cleanup_trtbl[] = {
	{ .return_value = -1 ,

	 .flag = SM_TERM }
};

# 51 "src/client/sysint/sys-list-eattr.sm"


PVFS_error PVFS_isys_listeattr(
        PVFS_object_ref ref,
        PVFS_ds_position token,
        int32_t nkey,
        const PVFS_credentials *credentials,
        PVFS_sysresp_listeattr *resp_p,
        PVFS_sys_op_id *op_id,
        PVFS_hint hints,
        void *user_ptr)
{
    int ret = -PVFS_EINVAL;
    PINT_smcb *smcb = NULL;
    PINT_client_sm *sm_p = NULL;

    gossip_debug(GOSSIP_CLIENT_DEBUG,
                 "PINT_isys_listeattr entered\n");

    if ((ref.handle == PVFS_HANDLE_NULL) ||
       (ref.fs_id == PVFS_FS_ID_NULL) || (resp_p == NULL))
    {
        gossip_err("invalid (NULL) required argument\n");
	return ret;
    }

    PINT_smcb_alloc(&smcb, PVFS_SYS_LISTEATTR,
             sizeof(struct PINT_client_sm),
             client_op_state_get_machine,
             client_state_machine_terminate,
             pint_client_sm_context);
    if (smcb == NULL)
    {
        return -PVFS_ENOMEM;
    }
    sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);

    PINT_init_msgarray_params(sm_p, ref.fs_id);
    PINT_init_sysint_credentials(sm_p->cred_p, credentials);
    sm_p->u.listeattr.nkey = nkey;
    sm_p->u.listeattr.resp_p = resp_p;
    sm_p->u.listeattr.pos_token = token;
    sm_p->error_code = 0;
    sm_p->object_ref = ref;
    PVFS_hint_copy(hints, &sm_p->hints);

    return PINT_client_state_machine_post(
            smcb,  op_id, user_ptr);
}

PVFS_error PVFS_sys_listeattr(
        PVFS_object_ref ref,
        PVFS_ds_position token,
        int32_t nkey,
        const PVFS_credentials *credentials,
        PVFS_sysresp_listeattr *resp_p,
        PVFS_hint hints)
{
    PVFS_error ret = -PVFS_EINVAL, error = 0;
    PVFS_sys_op_id op_id;

    gossip_debug(GOSSIP_CLIENT_DEBUG, "PVFS_sys_listeattr entered\n");

    ret = PVFS_isys_listeattr(ref, token, nkey, credentials,
            resp_p, &op_id, hints, NULL);

    if (ret)
    {
        PVFS_perror_gossip("PVFS_isys_listeattr call", ret);
        error = ret;
    }
    else
    {
        ret = PVFS_sys_wait(op_id, "listeattr", &error);
        if (ret)
        {
             PVFS_perror_gossip("PVFS_sys_wait call", ret);
             error = ret;
        }
    }

    PINT_sys_release(op_id);
    return error;
}

static PINT_sm_action list_eattr_setup_msgpair(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret = -PVFS_EINVAL;
    int i;
    PINT_sm_msgpair_state *msg_p;

    gossip_debug(GOSSIP_CLIENT_DEBUG,
		"list_eattr state: list_eattr_setup_msgpair\n");

    /* this will be the array of key buffer sizes */
    sm_p->u.listeattr.size_array =
             (PVFS_size *)malloc(sm_p->u.listeattr.nkey *
                                 sizeof (PVFS_size));

    for (i = 0; i < sm_p->u.listeattr.nkey; i++)
        sm_p->u.listeattr.size_array[i] =
            sm_p->u.listeattr.resp_p->key_array[i].buffer_sz;

    PINT_msgpair_init(&sm_p->msgarray_op);
    msg_p = &sm_p->msgarray_op.msgpair;

    PINT_SERVREQ_LISTEATTR_FILL(
            msg_p->req,
            (*sm_p->cred_p),
            sm_p->object_ref.fs_id,
            sm_p->object_ref.handle,
            sm_p->u.listeattr.pos_token,
            sm_p->u.listeattr.nkey,
            sm_p->u.listeattr.size_array,
            sm_p->hints
            );

    msg_p->fs_id = sm_p->object_ref.fs_id;
    msg_p->handle = sm_p->object_ref.handle;
    msg_p->retry_flag = PVFS_MSGPAIR_RETRY;
    msg_p->comp_fn = list_eattr_comp_fn;

    ret = PINT_cached_config_map_to_server(
            &msg_p->svr_addr,
            msg_p->handle,
            msg_p->fs_id);

    if (ret)
    {
        gossip_err("Failed to map meta server address\n");
        js_p->error_code = 0;
    }

    PINT_sm_push_frame(smcb, 0, &sm_p->msgarray_op);
    return SM_ACTION_COMPLETE;
}

static PINT_sm_action list_eattr_cleanup(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    gossip_debug(GOSSIP_CLIENT_DEBUG,
		"list_eattr state: list_eattr_cleanup\n");
    /* free the array malloc'd above */
    free(sm_p->u.listeattr.size_array);
    sm_p->error_code  = js_p->error_code;

    PINT_SET_OP_COMPLETE;
    return SM_ACTION_TERMINATE;
}

static int list_eattr_comp_fn(
    void *v_p,
    struct PVFS_server_resp *resp_p,
    int i)
{
    int ret = 0;
    PINT_smcb *smcb = v_p;
    PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_MSGPAIR_PARENT_SM);

    gossip_debug(GOSSIP_CLIENT_DEBUG,
		"list_eattr completion fn: list_eattr_comp_fn\n");

    /* only posted one msgpair */
    assert(i==0);

    if (resp_p->status != 0)
    {
        return resp_p->status;
    }
    if (sm_p->msgarray_op.msgarray[i].op_status != 0)
    {
        return sm_p->msgarray_op.msgarray[i].op_status;
    }
    sm_p->u.listeattr.resp_p->token = resp_p->u.listeattr.token;
    sm_p->u.listeattr.resp_p->nkey  = resp_p->u.listeattr.nkey;

    /* If we requested more than 0 keys */
    if (sm_p->u.listeattr.nkey)
    {
        gossip_debug(GOSSIP_LISTEATTR_DEBUG,"listeattr returned %d instead of %d keys\n",
                sm_p->u.listeattr.resp_p->nkey, sm_p->u.listeattr.nkey);
    }
    /* Else we use the token value as the total number of keys */
    else
    {
        gossip_debug(GOSSIP_LISTEATTR_DEBUG,"listeattr returned %d keys\n",
                sm_p->u.listeattr.resp_p->nkey);
        /* Copy the token and reset it */
        sm_p->u.listeattr.resp_p->nkey = sm_p->u.listeattr.resp_p->token;
        sm_p->u.listeattr.resp_p->token = PVFS_ITERATE_START;
        return 0;
    }
    if (sm_p->u.listeattr.resp_p->nkey > 0)
    {
        int k;
        if (!resp_p->u.listeattr.key)
        {
            gossip_err("Successful call returned NULL value list\n");
            return -1;
        }
        for (k = 0; k < sm_p->u.listeattr.resp_p->nkey; k++)
        {
            gossip_debug(GOSSIP_LISTEATTR_DEBUG,"resp_read_sz = %d\n",
                    resp_p->u.listeattr.key[k].read_sz);
            gossip_debug(GOSSIP_LISTEATTR_DEBUG,"resp_buff_sz = %d\n",
                    resp_p->u.listeattr.key[k].buffer_sz);
            gossip_debug(GOSSIP_LISTEATTR_DEBUG,"sm_buff_sz = %d\n",
                    sm_p->u.listeattr.resp_p->key_array[k].buffer_sz);
            gossip_debug(GOSSIP_LISTEATTR_DEBUG,"resp_buff_ = %s\n",
                    (char *)resp_p->u.listeattr.key[k].buffer);
            /* put the actual read_sz in its place */
            sm_p->u.listeattr.resp_p->key_array[k].read_sz =
                    resp_p->u.listeattr.key[k].buffer_sz;
            /* check for too big a return message */
            if (sm_p->u.listeattr.resp_p->key_array[k].read_sz <=
                    sm_p->u.listeattr.resp_p->key_array[k].buffer_sz)
            {
                gossip_debug(GOSSIP_LISTEATTR_DEBUG,"copying\n");
                memcpy(sm_p->u.listeattr.resp_p->key_array[k].buffer,
                        resp_p->u.listeattr.key[k].buffer,
                        sm_p->u.listeattr.resp_p->key_array[k].read_sz);
            }
            else /* oops, error! returned too much data */
            {
                gossip_debug(GOSSIP_LISTEATTR_DEBUG,"clearing on error\n");
                memset(sm_p->u.listeattr.resp_p->key_array[k].buffer,0,
                        sm_p->u.listeattr.resp_p->key_array[k].buffer_sz);
                /* record an error, but keep going */
                ret = -PVFS_EMSGSIZE;
            }
        }
    }
    return ret;
}

/*
 * Local variables:
 *  mode: c
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ft=c ts=8 sts=4 sw=4 expandtab
 */

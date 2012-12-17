/* WARNING: THIS FILE IS AUTOMATICALLY GENERATED FROM A .SM FILE.
 * Changes made here will certainly be overwritten.
 */

/* 
 * (C) 2001 Clemson University and The University of Chicago 
 *
 * See COPYING in top-level directory.
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#include "state-machine.h"
#include "client-state-machine.h"
#include "job-time-mgr.h"

extern job_context_id pint_client_sm_context;


static PINT_sm_action sysdev_unexp_post(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_post;
static struct PINT_pjmp_tbl_s ST_post_pjtbl[];
static struct PINT_tran_tbl_s ST_post_trtbl[];

static PINT_sm_action sysdev_unexp_cleanup(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_cleanup;
static struct PINT_pjmp_tbl_s ST_cleanup_pjtbl[];
static struct PINT_tran_tbl_s ST_cleanup_trtbl[];

struct PINT_state_machine_s pvfs2_sysdev_unexp_sm = {
	.name = "pvfs2_sysdev_unexp_sm",
	.first_state = &ST_post
};

static struct PINT_state_s ST_post = {
	 .state_name = "post" ,
	 .parent_machine = &pvfs2_sysdev_unexp_sm ,
	 .flag = SM_RUN ,
	 .action.func = sysdev_unexp_post ,
	 .pjtbl = NULL ,
	 .trtbl = ST_post_trtbl 
};

static struct PINT_tran_tbl_s ST_post_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_cleanup }
};

static struct PINT_state_s ST_cleanup = {
	 .state_name = "cleanup" ,
	 .parent_machine = &pvfs2_sysdev_unexp_sm ,
	 .flag = SM_RUN ,
	 .action.func = sysdev_unexp_cleanup ,
	 .pjtbl = NULL ,
	 .trtbl = ST_cleanup_trtbl 
};

static struct PINT_tran_tbl_s ST_cleanup_trtbl[] = {
	{ .return_value = -1 ,

	 .flag = SM_TERM }
};

# 37 "src/client/sysint/pint-sysdev-unexp.sm"


/* sysdev_unexp_cleanup()
 *
 * cleans up any resources consumed by this state machine and ends
 * execution of the machine
 */
static int sysdev_unexp_cleanup(struct PINT_smcb *smcb, job_status_s* js_p)
{
    return SM_ACTION_TERMINATE;
}

/* sysdev_unexp_post()
 *
 * posts an unexpected message job for the client
 */
static int sysdev_unexp_post(struct PINT_smcb *smcb, job_status_s* js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    PVFS_error ret;
    job_id_t tmpid;

    memset(js_p, 0, sizeof(job_status_s));
    ret = job_dev_unexp(sm_p->u.sysdev_unexp.info, (void *)smcb, 0, js_p,
                        &tmpid, JOB_NO_IMMED_COMPLETE, pint_client_sm_context);
    if (ret < 0)
    {
        PVFS_perror_gossip("PINT_sys_dev_unexp failed", ret);
        return SM_ACTION_COMPLETE;
    }
    return SM_ACTION_DEFERRED;
}

PVFS_error PINT_sys_dev_unexp(
    struct PINT_dev_unexp_info *info,
    job_status_s *jstat,
    PVFS_sys_op_id *op_id,
    void *user_ptr)
{
    PVFS_error ret = -PVFS_EINVAL;
    PINT_smcb *smcb = NULL;
    PINT_client_sm *sm_p = NULL;

    gossip_debug(GOSSIP_CLIENT_DEBUG, "PINT_sys_dev_unexp\n");

    /* we require more input args than the regular post method above */
    if (!info || !jstat || !op_id)
    {
        return -PVFS_EINVAL;
    }
                                                                                
    ret = PINT_smcb_alloc(&smcb, PVFS_DEV_UNEXPECTED,
            sizeof(struct PINT_client_sm),
            client_op_state_get_machine,
            client_state_machine_terminate,
            pint_client_sm_context);
    if (ret < 0)
    {
        gossip_lerr("Error: failed to allocate SMCB "
                    "of op type %x\n", PVFS_DEV_UNEXPECTED);
        return ret;
    }
    sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    sm_p->u.sysdev_unexp.info = info;
    sm_p->cred_p = NULL;

    ret = PINT_client_state_machine_post(smcb, op_id, user_ptr);

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
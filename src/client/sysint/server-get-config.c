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

static int server_parse_config(
    struct server_configuration_s *config,
    char *fs_config_buf,
    uint32_t fs_config_buf_size);

static int server_get_config_comp_fn(
    void *v_p, struct PVFS_server_resp *resp_p, int i);


static PINT_sm_action server_get_config_setup_msgpair(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_setup_msgpair;
static struct PINT_pjmp_tbl_s ST_setup_msgpair_pjtbl[];
static struct PINT_tran_tbl_s ST_setup_msgpair_trtbl[];
static struct PINT_state_s ST_xfer_msgpair;
static struct PINT_pjmp_tbl_s ST_xfer_msgpair_pjtbl[];
static struct PINT_tran_tbl_s ST_xfer_msgpair_trtbl[];

static PINT_sm_action server_get_config_parse(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_parse;
static struct PINT_pjmp_tbl_s ST_parse_pjtbl[];
static struct PINT_tran_tbl_s ST_parse_trtbl[];

static PINT_sm_action server_get_config_cleanup(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_cleanup;
static struct PINT_pjmp_tbl_s ST_cleanup_pjtbl[];
static struct PINT_tran_tbl_s ST_cleanup_trtbl[];

struct PINT_state_machine_s pvfs2_server_get_config_nested_sm = {
	.name = "pvfs2_server_get_config_nested_sm",
	.first_state = &ST_setup_msgpair
};

static struct PINT_state_s ST_setup_msgpair = {
	 .state_name = "setup_msgpair" ,
	 .parent_machine = &pvfs2_server_get_config_nested_sm ,
	 .flag = SM_RUN ,
	 .action.func = server_get_config_setup_msgpair ,
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
	 .parent_machine = &pvfs2_server_get_config_nested_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_msgpairarray_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_xfer_msgpair_trtbl 
};

static struct PINT_tran_tbl_s ST_xfer_msgpair_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_parse },
	{ .return_value = -1 ,
	 .next_state = &ST_cleanup }
};

static struct PINT_state_s ST_parse = {
	 .state_name = "parse" ,
	 .parent_machine = &pvfs2_server_get_config_nested_sm ,
	 .flag = SM_RUN ,
	 .action.func = server_get_config_parse ,
	 .pjtbl = NULL ,
	 .trtbl = ST_parse_trtbl 
};

static struct PINT_tran_tbl_s ST_parse_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_cleanup }
};

static struct PINT_state_s ST_cleanup = {
	 .state_name = "cleanup" ,
	 .parent_machine = &pvfs2_server_get_config_nested_sm ,
	 .flag = SM_RUN ,
	 .action.func = server_get_config_cleanup ,
	 .pjtbl = NULL ,
	 .trtbl = ST_cleanup_trtbl 
};

static struct PINT_tran_tbl_s ST_cleanup_trtbl[] = {
	{ .return_value = -1 ,
	 .flag = SM_RETURN }
};

static struct PINT_state_s ST_run_nested;
static struct PINT_pjmp_tbl_s ST_run_nested_pjtbl[];
static struct PINT_tran_tbl_s ST_run_nested_trtbl[];

static PINT_sm_action server_get_config_parent_cleanup(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_parent_cleanup;
static struct PINT_pjmp_tbl_s ST_parent_cleanup_pjtbl[];
static struct PINT_tran_tbl_s ST_parent_cleanup_trtbl[];

struct PINT_state_machine_s pvfs2_server_get_config_sm = {
	.name = "pvfs2_server_get_config_sm",
	.first_state = &ST_run_nested
};

static struct PINT_state_s ST_run_nested = {
	 .state_name = "run_nested" ,
	 .parent_machine = &pvfs2_server_get_config_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_server_get_config_nested_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_run_nested_trtbl 
};

static struct PINT_tran_tbl_s ST_run_nested_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_parent_cleanup }
};

static struct PINT_state_s ST_parent_cleanup = {
	 .state_name = "parent_cleanup" ,
	 .parent_machine = &pvfs2_server_get_config_sm ,
	 .flag = SM_RUN ,
	 .action.func = server_get_config_parent_cleanup ,
	 .pjtbl = NULL ,
	 .trtbl = ST_parent_cleanup_trtbl 
};

static struct PINT_tran_tbl_s ST_parent_cleanup_trtbl[] = {
	{ .return_value = -1 ,

	 .flag = SM_TERM }
};

# 76 "src/client/sysint/server-get-config.sm"


/*
  given mount information, retrieve the server's configuration by
  issuing a getconfig operation.  on successful response, we parse the
  configuration and fill in the config object specified.

  returns 0 on success, -errno on error
*/
int PINT_server_get_config(
    struct server_configuration_s *config,
    struct PVFS_sys_mntent* mntent_p,
    PVFS_hint hints)
{
    int ret = -PVFS_EINVAL;
    PINT_smcb *smcb = NULL;
    PINT_client_sm *sm_p = NULL;
    PVFS_error error = 0;
    PVFS_credentials creds;
    PVFS_sys_op_id op_id;

    gossip_debug(GOSSIP_CLIENT_DEBUG,
                 "PINT_server_get_config entered\n");

    if (!config || !mntent_p)
    {
	return ret;
    }

    PVFS_util_gen_credentials(&creds);

    gossip_debug(GOSSIP_CLIENT_DEBUG, "asked for fs name = %s\n",
                 mntent_p->pvfs_fs_name);

    PINT_smcb_alloc(&smcb, PVFS_SERVER_GET_CONFIG,
             sizeof(struct PINT_client_sm),
             client_op_state_get_machine,
             client_state_machine_terminate,
             pint_client_sm_context);
    if (smcb == NULL)
    {
        return -PVFS_ENOMEM;
    }
    sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);

    /* NOTE: we set these fields manually here rather than use
     * PINT_init_msgarray_params(), because we don't yet have a server
     * configuration file to override default parameters.
     */
    sm_p->msgarray_op.params.job_context = pint_client_sm_context;
    sm_p->msgarray_op.params.job_timeout = 30;   /* 30 second job timeout */
    sm_p->msgarray_op.params.retry_delay = 2000; /* 2 second retry delay */
    sm_p->msgarray_op.params.retry_limit = 5;    /* retry up to 5 times */

    PINT_msgpair_init(&sm_p->msgarray_op);
    PINT_init_sysint_credentials(sm_p->cred_p, &creds);
    sm_p->u.get_config.mntent = mntent_p;
    sm_p->u.get_config.config = config;
    
    PVFS_hint_copy(hints, &sm_p->hints);

    ret = PINT_client_state_machine_post(smcb, &op_id, NULL);
    if (ret)
    {
        PVFS_perror_gossip("PINT_client_state_machine_post call", ret);
        error = ret;
    }
    else
    {
        ret = PVFS_sys_wait(op_id, "X-get_config", &error);
        if (ret)
        {
            PVFS_perror_gossip("PVFS_sys_wait call", ret);
            error = ret;
        }
    }

    PINT_sys_release(op_id);
    return(error);
}

static int server_parse_config(
    struct server_configuration_s *config,
    char *fs_config_buf,
    uint32_t fs_config_buf_size)
{
    int ret = 1, template_index = 1;
    int fs_fd = 0;
    char fs_template_array[2][64] =
    {
        ".__pvfs_fs_configXXXXXX",
        "/tmp/.__pvfs_fs_configXXXXXX"
    };
    char *fs_template = NULL;

    if (config)
    {
        assert(fs_config_buf);

        while(1)
        {
            assert(template_index > -1);
            fs_template = fs_template_array[template_index];

            fs_fd = mkstemp(fs_template);
            if (fs_fd != -1)
            {
                break;
            }
            else if ((--template_index) < 0)
            {
                gossip_err("Error: Cannot create temporary "
                           "configuration files!\n");
                return ret;
            }
        }

        assert(!fs_config_buf[fs_config_buf_size - 1]);

        if (write(fs_fd,fs_config_buf, 
		    (fs_config_buf_size - 1)) == (fs_config_buf_size - 1))
        {
            ret = PINT_parse_config(config, fs_template, NULL);
        }
        else
        {
            gossip_err("Failed to write fs.conf buffer to temp file: %s: %s\n",
                       fs_template, strerror(errno));
        }
    
        close(fs_fd);

        remove(fs_template);
    }
    return ret;
}

static PINT_sm_action server_get_config_setup_msgpair(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret = -PVFS_EINVAL;
    PINT_sm_msgpair_state *msg_p = NULL;
    PVFS_BMI_addr_t serv_addr;

    gossip_debug(GOSSIP_CLIENT_DEBUG,
                 "get_config state: server_get_config_setup_msgpair\n");

    PINT_msgpair_init(&sm_p->msgarray_op);
    msg_p = &sm_p->msgarray_op.msgpair;

    if (ENCODING_IS_VALID(sm_p->u.get_config.mntent->encoding))
    {
        msg_p->enc_type = sm_p->u.get_config.mntent->encoding;
    }

    ret = BMI_addr_lookup(&serv_addr,
                          sm_p->u.get_config.mntent->the_pvfs_config_server);
    if (ret < 0)
    {
        gossip_lerr("Failed to resolve BMI address %s\n",
                    sm_p->u.get_config.mntent->the_pvfs_config_server);
        js_p->error_code = ret;
        return SM_ACTION_COMPLETE;
    }

    PINT_SERVREQ_GETCONFIG_FILL(msg_p->req, *sm_p->cred_p, sm_p->hints);

    msg_p->fs_id = PVFS_FS_ID_NULL;
    msg_p->handle = PVFS_HANDLE_NULL;
    msg_p->retry_flag = PVFS_MSGPAIR_RETRY;
    msg_p->comp_fn = server_get_config_comp_fn;
    msg_p->svr_addr = serv_addr;

    PINT_sm_push_frame(smcb, 0, &sm_p->msgarray_op);
    js_p->error_code = 0;
    return SM_ACTION_COMPLETE;
}

static PINT_sm_action server_get_config_cleanup(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    if (sm_p && !sm_p->u.get_config.persist_config_buffers)
    {
	free(sm_p->u.get_config.fs_config_buf);
        sm_p->u.get_config.fs_config_buf = NULL;
    }

    /* preserve js_p->error_code */

    return SM_ACTION_COMPLETE;
}

static PINT_sm_action server_get_config_parse(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret = -1;
    struct filesystem_configuration_s* cur_fs = NULL;

    if(sm_p->u.get_config.config)
    {
        ret = server_parse_config(
            sm_p->u.get_config.config,  sm_p->u.get_config.fs_config_buf,
            sm_p->u.get_config.fs_config_buf_size);
        if (ret)
        {
            gossip_err("Failed to get_config from host %s\n",
                       sm_p->u.get_config.mntent->the_pvfs_config_server); 
            js_p->error_code = ret;
            return SM_ACTION_COMPLETE;
        }
    }

    if(sm_p->u.get_config.mntent->fs_id == PVFS_FS_ID_NULL)
    {
        cur_fs = PINT_config_find_fs_name(sm_p->u.get_config.config, 
            sm_p->u.get_config.mntent->pvfs_fs_name);
        if (!cur_fs)
        {
            gossip_err("Warning:\n Cannot retrieve information about "
                    "filesystem %s at tab entry: %s\n",
                    sm_p->u.get_config.mntent->pvfs_fs_name,
                    sm_p->u.get_config.mntent->the_pvfs_config_server);

            /*
              if the device has no space left on it, we can't save
              the config file for parsing and get a failure; make
              a note of that possibility here
            */ 
            gossip_err("\nHINTS: If you're sure that your pvfstab file "
                       "contains valid information,\n please make sure "
                       "that you are not out of disk space and that you "
                       "have\n write permissions in the current "
                       "directory or in the /tmp directory\n\n");

            js_p->error_code = -PVFS_ENODEV;
            return SM_ACTION_COMPLETE;
        }

        sm_p->u.get_config.mntent->fs_id = cur_fs->coll_id;
        cur_fs->flowproto = sm_p->u.get_config.mntent->flowproto;
        cur_fs->encoding = sm_p->u.get_config.mntent->encoding;
    }

    js_p->error_code = 0;
    return SM_ACTION_COMPLETE;
}


static PINT_sm_action server_get_config_parent_cleanup(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    sm_p->error_code  = js_p->error_code;
    PINT_SET_OP_COMPLETE;

    return SM_ACTION_DEFERRED;
}


static int server_get_config_comp_fn(
    void *v_p,
    struct PVFS_server_resp *resp_p,
    int i)
{
    PINT_smcb *smcb = v_p;
    PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_MSGPAIR_PARENT_SM);

    /* only posted one msgpair */
    assert(i==0);

    /* if this particular request was successful, then store the server
     * response and let the caller sort it out */

    if (sm_p->msgarray_op.msgarray[i].op_status == 0)
    {
	sm_p->u.get_config.fs_config_buf = 
		strdup(resp_p->u.getconfig.fs_config_buf);
	sm_p->u.get_config.fs_config_buf_size = 
		resp_p->u.getconfig.fs_config_buf_size;
    }

    /* if this is the last response, check all of the status values
     * and return error code if any requests failed
     */
    if (i == (sm_p->msgarray_op.count -1))
    {
        return PINT_msgarray_status(&sm_p->msgarray_op);
    }
    return 0;
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

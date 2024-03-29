/* WARNING: THIS FILE IS AUTOMATICALLY GENERATED FROM A .SM FILE.
 * Changes made here will certainly be overwritten.
 */

/* 
 * (C) 2003 Clemson University and The University of Chicago 
 *
 * See COPYING in top-level directory.
 */

/** \file
 *  \ingroup sysint
 *
 *  PVFS2 system interface routines for obtaining attributes of an object
 *  (file or directory).
 */
#include <string.h>
#include <assert.h>

#include "client-state-machine.h"
#include "pvfs2-debug.h"
#include "job.h"
#include "gossip.h"
#include "str-utils.h"
#include "pint-util.h"
#include "pvfs2-util.h"
#include "pint-cached-config.h"
#include "PINT-reqproto-encode.h"
#include "pvfs2-internal.h"

/* pvfs2_client_getattr_sm
 *
 * The sm_p->msgpair structure is used to get the attributes of the
 * object itself.  We convert the original attribute mask (in
 * sm_p->u.getattr.attrmask) to ask for datafile and distribution info
 * if the user asked for file size (PVFS_ATTR_SYS_SIZE).  This allows
 * us to obtain this information (if the object turns out to be a
 * metafile) so that we can later look up the datafile sizes and
 * calculate the overall file size.
 *
 * The sm_p->msgpairarray is used to get datafile sizes, if it turns
 * out that we need them.  This space will also need to be freed, if
 * we grab these sizes.
 */

extern job_context_id pint_client_sm_context;

enum
{
    GETATTR_ACACHE_MISS = 1,
    GETATTR_NEED_DATAFILE_SIZES = 2
};

/* completion function prototypes */
static int getattr_object_getattr_comp_fn(
    void *v_p, struct PVFS_server_resp *resp_p, int index);
static int getattr_datafile_getattr_comp_fn(
    void *v_p, struct PVFS_server_resp *resp_p, int index);


static PINT_sm_action getattr_datafile_getattr_setup_msgpairarray(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_datafile_getattr_setup_msgpairarray;
static struct PINT_pjmp_tbl_s ST_datafile_getattr_setup_msgpairarray_pjtbl[];
static struct PINT_tran_tbl_s ST_datafile_getattr_setup_msgpairarray_trtbl[];
static struct PINT_state_s ST_datafile_getattr_xfer_msgpairarray;
static struct PINT_pjmp_tbl_s ST_datafile_getattr_xfer_msgpairarray_pjtbl[];
static struct PINT_tran_tbl_s ST_datafile_getattr_xfer_msgpairarray_trtbl[];

static PINT_sm_action getattr_datafile_getattr_failure(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_datafile_getattr_failure;
static struct PINT_pjmp_tbl_s ST_datafile_getattr_failure_pjtbl[];
static struct PINT_tran_tbl_s ST_datafile_getattr_failure_trtbl[];

static PINT_sm_action getattr_datafile_getattr_cleanup(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_datafile_getattr_cleanup;
static struct PINT_pjmp_tbl_s ST_datafile_getattr_cleanup_pjtbl[];
static struct PINT_tran_tbl_s ST_datafile_getattr_cleanup_trtbl[];

struct PINT_state_machine_s pvfs2_client_datafile_getattr_sizes_sm = {
	.name = "pvfs2_client_datafile_getattr_sizes_sm",
	.first_state = &ST_datafile_getattr_setup_msgpairarray
};

static struct PINT_state_s ST_datafile_getattr_setup_msgpairarray = {
	 .state_name = "datafile_getattr_setup_msgpairarray" ,
	 .parent_machine = &pvfs2_client_datafile_getattr_sizes_sm ,
	 .flag = SM_RUN ,
	 .action.func = getattr_datafile_getattr_setup_msgpairarray ,
	 .pjtbl = NULL ,
	 .trtbl = ST_datafile_getattr_setup_msgpairarray_trtbl 
};

static struct PINT_tran_tbl_s ST_datafile_getattr_setup_msgpairarray_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_datafile_getattr_xfer_msgpairarray },
	{ .return_value = -1 ,
	 .next_state = &ST_datafile_getattr_failure }
};

static struct PINT_state_s ST_datafile_getattr_xfer_msgpairarray = {
	 .state_name = "datafile_getattr_xfer_msgpairarray" ,
	 .parent_machine = &pvfs2_client_datafile_getattr_sizes_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_msgpairarray_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_datafile_getattr_xfer_msgpairarray_trtbl 
};

static struct PINT_tran_tbl_s ST_datafile_getattr_xfer_msgpairarray_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_datafile_getattr_cleanup },
	{ .return_value = -1 ,
	 .next_state = &ST_datafile_getattr_failure }
};

static struct PINT_state_s ST_datafile_getattr_failure = {
	 .state_name = "datafile_getattr_failure" ,
	 .parent_machine = &pvfs2_client_datafile_getattr_sizes_sm ,
	 .flag = SM_RUN ,
	 .action.func = getattr_datafile_getattr_failure ,
	 .pjtbl = NULL ,
	 .trtbl = ST_datafile_getattr_failure_trtbl 
};

static struct PINT_tran_tbl_s ST_datafile_getattr_failure_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_datafile_getattr_cleanup }
};

static struct PINT_state_s ST_datafile_getattr_cleanup = {
	 .state_name = "datafile_getattr_cleanup" ,
	 .parent_machine = &pvfs2_client_datafile_getattr_sizes_sm ,
	 .flag = SM_RUN ,
	 .action.func = getattr_datafile_getattr_cleanup ,
	 .pjtbl = NULL ,
	 .trtbl = ST_datafile_getattr_cleanup_trtbl 
};

static struct PINT_tran_tbl_s ST_datafile_getattr_cleanup_trtbl[] = {
	{ .return_value = -1 ,
	 .flag = SM_RETURN }
};


static PINT_sm_action getattr_acache_lookup(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_acache_lookup;
static struct PINT_pjmp_tbl_s ST_acache_lookup_pjtbl[];
static struct PINT_tran_tbl_s ST_acache_lookup_trtbl[];

static PINT_sm_action getattr_object_getattr_setup_msgpair(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_object_getattr_setup_msgpair;
static struct PINT_pjmp_tbl_s ST_object_getattr_setup_msgpair_pjtbl[];
static struct PINT_tran_tbl_s ST_object_getattr_setup_msgpair_trtbl[];
static struct PINT_state_s ST_object_getattr_xfer_msgpair;
static struct PINT_pjmp_tbl_s ST_object_getattr_xfer_msgpair_pjtbl[];
static struct PINT_tran_tbl_s ST_object_getattr_xfer_msgpair_trtbl[];

static PINT_sm_action getattr_acache_insert(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_acache_insert;
static struct PINT_pjmp_tbl_s ST_acache_insert_pjtbl[];
static struct PINT_tran_tbl_s ST_acache_insert_trtbl[];

static PINT_sm_action getattr_object_getattr_failure(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_object_getattr_failure;
static struct PINT_pjmp_tbl_s ST_object_getattr_failure_pjtbl[];
static struct PINT_tran_tbl_s ST_object_getattr_failure_trtbl[];
static struct PINT_state_s ST_datafile_get_sizes;
static struct PINT_pjmp_tbl_s ST_datafile_get_sizes_pjtbl[];
static struct PINT_tran_tbl_s ST_datafile_get_sizes_trtbl[];

static PINT_sm_action getattr_cleanup(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_cleanup;
static struct PINT_pjmp_tbl_s ST_cleanup_pjtbl[];
static struct PINT_tran_tbl_s ST_cleanup_trtbl[];

struct PINT_state_machine_s pvfs2_client_getattr_sm = {
	.name = "pvfs2_client_getattr_sm",
	.first_state = &ST_acache_lookup
};

static struct PINT_state_s ST_acache_lookup = {
	 .state_name = "acache_lookup" ,
	 .parent_machine = &pvfs2_client_getattr_sm ,
	 .flag = SM_RUN ,
	 .action.func = getattr_acache_lookup ,
	 .pjtbl = NULL ,
	 .trtbl = ST_acache_lookup_trtbl 
};

static struct PINT_tran_tbl_s ST_acache_lookup_trtbl[] = {
	{ .return_value = GETATTR_ACACHE_MISS ,
	 .next_state = &ST_object_getattr_setup_msgpair },
	{ .return_value = GETATTR_NEED_DATAFILE_SIZES ,
	 .next_state = &ST_datafile_get_sizes },
	{ .return_value = -1 ,
	 .next_state = &ST_cleanup }
};

static struct PINT_state_s ST_object_getattr_setup_msgpair = {
	 .state_name = "object_getattr_setup_msgpair" ,
	 .parent_machine = &pvfs2_client_getattr_sm ,
	 .flag = SM_RUN ,
	 .action.func = getattr_object_getattr_setup_msgpair ,
	 .pjtbl = NULL ,
	 .trtbl = ST_object_getattr_setup_msgpair_trtbl 
};

static struct PINT_tran_tbl_s ST_object_getattr_setup_msgpair_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_object_getattr_xfer_msgpair },
	{ .return_value = -1 ,
	 .next_state = &ST_cleanup }
};

static struct PINT_state_s ST_object_getattr_xfer_msgpair = {
	 .state_name = "object_getattr_xfer_msgpair" ,
	 .parent_machine = &pvfs2_client_getattr_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_msgpairarray_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_object_getattr_xfer_msgpair_trtbl 
};

static struct PINT_tran_tbl_s ST_object_getattr_xfer_msgpair_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_acache_insert },
	{ .return_value = GETATTR_NEED_DATAFILE_SIZES ,
	 .next_state = &ST_datafile_get_sizes },
	{ .return_value = -1 ,
	 .next_state = &ST_object_getattr_failure }
};

static struct PINT_state_s ST_acache_insert = {
	 .state_name = "acache_insert" ,
	 .parent_machine = &pvfs2_client_getattr_sm ,
	 .flag = SM_RUN ,
	 .action.func = getattr_acache_insert ,
	 .pjtbl = NULL ,
	 .trtbl = ST_acache_insert_trtbl 
};

static struct PINT_tran_tbl_s ST_acache_insert_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_cleanup }
};

static struct PINT_state_s ST_object_getattr_failure = {
	 .state_name = "object_getattr_failure" ,
	 .parent_machine = &pvfs2_client_getattr_sm ,
	 .flag = SM_RUN ,
	 .action.func = getattr_object_getattr_failure ,
	 .pjtbl = NULL ,
	 .trtbl = ST_object_getattr_failure_trtbl 
};

static struct PINT_tran_tbl_s ST_object_getattr_failure_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_cleanup }
};

static struct PINT_state_s ST_datafile_get_sizes = {
	 .state_name = "datafile_get_sizes" ,
	 .parent_machine = &pvfs2_client_getattr_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_client_datafile_getattr_sizes_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_datafile_get_sizes_trtbl 
};

static struct PINT_tran_tbl_s ST_datafile_get_sizes_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_acache_insert },
	{ .return_value = -1 ,
	 .next_state = &ST_cleanup }
};

static struct PINT_state_s ST_cleanup = {
	 .state_name = "cleanup" ,
	 .parent_machine = &pvfs2_client_getattr_sm ,
	 .flag = SM_RUN ,
	 .action.func = getattr_cleanup ,
	 .pjtbl = NULL ,
	 .trtbl = ST_cleanup_trtbl 
};

static struct PINT_tran_tbl_s ST_cleanup_trtbl[] = {
	{ .return_value = -1 ,
	 .flag = SM_RETURN }
};

static struct PINT_state_s ST_dowork;
static struct PINT_pjmp_tbl_s ST_dowork_pjtbl[];
static struct PINT_tran_tbl_s ST_dowork_trtbl[];

static PINT_sm_action getattr_set_sys_response(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_set_sys_response;
static struct PINT_pjmp_tbl_s ST_set_sys_response_pjtbl[];
static struct PINT_tran_tbl_s ST_set_sys_response_trtbl[];

struct PINT_state_machine_s pvfs2_client_sysint_getattr_sm = {
	.name = "pvfs2_client_sysint_getattr_sm",
	.first_state = &ST_dowork
};

static struct PINT_state_s ST_dowork = {
	 .state_name = "dowork" ,
	 .parent_machine = &pvfs2_client_sysint_getattr_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_client_getattr_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_dowork_trtbl 
};

static struct PINT_tran_tbl_s ST_dowork_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_set_sys_response }
};

static struct PINT_state_s ST_set_sys_response = {
	 .state_name = "set_sys_response" ,
	 .parent_machine = &pvfs2_client_sysint_getattr_sm ,
	 .flag = SM_RUN ,
	 .action.func = getattr_set_sys_response ,
	 .pjtbl = NULL ,
	 .trtbl = ST_set_sys_response_trtbl 
};

static struct PINT_tran_tbl_s ST_set_sys_response_trtbl[] = {
	{ .return_value = -1 ,

	 .flag = SM_TERM }
};

# 154 "src/client/sysint/sys-getattr.sm"



/** Initiate retrieval of object attributes.
 */
PVFS_error PVFS_isys_getattr(
    PVFS_object_ref ref,
    uint32_t attrmask, 
    const PVFS_credentials *credentials,
    PVFS_sysresp_getattr *resp_p,
    PVFS_sys_op_id *op_id,
    PVFS_hint hints,
    void *user_ptr)
{
    PVFS_error ret = -PVFS_EINVAL;
    PINT_smcb *smcb = NULL;
    PINT_client_sm *sm_p = NULL;

    gossip_debug(GOSSIP_CLIENT_DEBUG, "PVFS_isys_getattr entered\n");

    if ((ref.handle == PVFS_HANDLE_NULL) ||
        (ref.fs_id == PVFS_FS_ID_NULL) || (resp_p == NULL))
    {
        gossip_err("invalid (NULL) required argument\n");
        return ret;
    }
    
    if (attrmask & ~(PVFS_ATTR_SYS_ALL))
    {
        gossip_err("invalid attrmask\n");
        return ret;
    }

    PINT_smcb_alloc(&smcb, PVFS_SYS_GETATTR, 
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
    sm_p->error_code = 0;
    sm_p->object_ref = ref;
    sm_p->u.getattr.getattr_resp_p = resp_p;
    PVFS_hint_copy(hints, &sm_p->hints);
    PVFS_hint_add(&sm_p->hints, PVFS_HINT_HANDLE_NAME, sizeof(PVFS_handle), &ref.handle);
    
    PINT_SM_GETATTR_STATE_FILL(
        sm_p->getattr,
        ref,
        PVFS_util_sys_to_object_attr_mask(
            attrmask),
        PVFS_TYPE_NONE,
        0);

    return PINT_client_state_machine_post(
        smcb,  op_id, user_ptr);
}

/** Retrieve object attributes.
 */
PVFS_error PVFS_sys_getattr(
    PVFS_object_ref ref,
    uint32_t attrmask, 
    const PVFS_credentials *credentials,
    PVFS_sysresp_getattr *resp_p,
    PVFS_hint hints)
{
    PVFS_error ret, error;
    PVFS_sys_op_id op_id;

    gossip_debug(GOSSIP_CLIENT_DEBUG, "PVFS_sys_getattr entered\n");

    ret = PVFS_isys_getattr(ref, attrmask, credentials,
                            resp_p, &op_id, hints, NULL);
    if (ret)
    {
        PVFS_perror_gossip("PVFS_isys_getattr call", ret);
        return ret;
    }

    if(op_id != -1)
    {
        /* did not complete immediately, so we wait */

        ret = PVFS_sys_wait(op_id, "getattr", &error);
        if (ret)
        {
            PVFS_perror_gossip("PVFS_sys_wait call", ret);
        }
        if(error)
        {
            ret = error;
        }
        PINT_sys_release(op_id);
    }

    return ret;
}


/**
 * getattr_acache_lookup
 * @ingroup client_sm_getattr
 * 
 * This function is invoked as the first state action of the
 * getattr-dowork state machine.  It performs a lookup into the
 * attribute cache for the attribute in question, and returns
 * result codes for a hit or a miss.  
 * 
 * @param smcb This must be a valid client state machine handle, with
 * the @ref getattr field containing valid values for the
 * fsid/handle of the desired attribute (in object_ref), as well as the 
 * requested attribute mask (req_attrmask).
 *
 * @param js_p Contains the return code to be set by this function in the
 * @ref error_code field.  This determines the next state action to jump
 * to in the getattr-dowork state machine.  Possible values for js_p->error_code
 * are:
 * <ul>
 * <li><b>GETATTR_ACACHE_MISS</b> - The requested attribute was not found
 * in the attribute cache, or the attribute was found, but the attribute's
 * mask did not include values required by the requested mask (req_attrmask).
 * </li>
 * <li><b>GETATTR_NEED_DATAFILE_SIZES</b> - The requested attribute was found
 * in the attribute cache and the mask was sufficient, but the data file size
 * was requested for this handle, so we need to get that next.
 * </li>
 * <li><b>default</b> - The requested attribute was found in the attribute
 * cache and its mask values were sufficient.
 * </li>
 *
 * @return This function should always return 1 unless an error occurred
 * within the internals of the state machine.
 */
static PINT_sm_action getattr_acache_lookup(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    uint32_t trimmed_mask = 0;
    int missing_attrs;
    PVFS_object_ref object_ref;
    int ret = -1;
    int attr_status = -1;
    int size_status = -1;

    js_p->error_code = 0;

    object_ref = sm_p->getattr.object_ref;

    assert(object_ref.handle != PVFS_HANDLE_NULL);
    assert(object_ref.fs_id != PVFS_FS_ID_NULL);

    gossip_debug(GOSSIP_ACACHE_DEBUG, "%s: handle %llu fsid %d\n",
      __func__, llu(object_ref.handle), object_ref.fs_id);

   
    /* The sys attr mask request is converted to object
     * attr mask values for comparison with the cached
     */
    if(sm_p->getattr.req_attrmask & PVFS_ATTR_DATA_SIZE)
    {
	sm_p->getattr.req_attrmask |= PVFS_ATTR_META_ALL;
    }

    if(sm_p->getattr.flags & PINT_SM_GETATTR_BYPASS_CACHE)
    {
        gossip_debug(GOSSIP_ACACHE_DEBUG, "acache: forced acache miss: "
                    " [%llu]\n",
                      llu(object_ref.handle));
        js_p->error_code = GETATTR_ACACHE_MISS;
        return SM_ACTION_COMPLETE;
    }

    ret = PINT_acache_get_cached_entry(object_ref,
        &sm_p->getattr.attr,
        &attr_status,
        &sm_p->getattr.size,
        &size_status);
    if(ret < 0 || attr_status < 0)
    {
        gossip_debug(GOSSIP_ACACHE_DEBUG, "acache: clean acache miss: "
                    " [%llu]\n",
                      llu(object_ref.handle));
  
        js_p->error_code = GETATTR_ACACHE_MISS;
        return SM_ACTION_COMPLETE;
    }

    /* acache hit, check results */
  
    /* The sys attr mask request is converted to object
     * attr mask values for comparison with the cached
     * entry
     */
    trimmed_mask = sm_p->getattr.req_attrmask;
    /* the trimmed mask is used for making sure that we're only
     * checking attr bits that make sense for the object type
     * since the caller may have requested all attributes in
     * the case where it doesn't know what type of object we're
     * doing the getattr against.
     */
    if (sm_p->getattr.attr.objtype == PVFS_TYPE_METAFILE)
    {
        trimmed_mask &= (PVFS_ATTR_META_ALL |
                         PVFS_ATTR_META_UNSTUFFED |
                         PVFS_ATTR_DATA_SIZE |
                         PVFS_ATTR_COMMON_ALL);
    }
    else if (sm_p->getattr.attr.objtype == PVFS_TYPE_SYMLINK)
    {
        trimmed_mask &= (PVFS_ATTR_SYMLNK_ALL | PVFS_ATTR_COMMON_ALL);
    }
    else if (sm_p->getattr.attr.objtype == PVFS_TYPE_DIRECTORY)
    {
        trimmed_mask &= (PVFS_ATTR_COMMON_ALL | PVFS_ATTR_DIR_ALL);
    }
  
    /* trimmed_mask contains the list of attributes
     * requested for a particular object, 
     * while sm_p->getattr.attr.mask contains
     * the list of attributes cached for that object.
     * The cached attributes can be used if requested
     * is less than cached, i.e. all the attributes
     * we need are already cached.  So we need to do
     * a bitwise comparison of requested <= cached.
     *
     * xor of the two masks gives us the bits that are different,
     * and-ing that result with the requested mask gives us the
     * bits in the requested mask but not in the cached mask.
     */
    missing_attrs = ((trimmed_mask ^ sm_p->getattr.attr.mask) &
                     trimmed_mask);
    if((missing_attrs == PVFS_ATTR_DATA_SIZE && size_status == 0) ||
        (missing_attrs == 0))
    {
        /* nothing's missing, a hit! */
        gossip_debug(GOSSIP_ACACHE_DEBUG, "acache: acache hit "
                     "[%llu]\n", llu(object_ref.handle));
        js_p->error_code = 0;
        return SM_ACTION_COMPLETE;
    }
  
    /* check if the only thing missing is the file size, then
     * we don't need to do the object getattr operation, we only
     * need to do the datafile getattr operation, so we return
     * the GETATTR_NEED_DATAFILE_SIZES error code which will make
     * the getattr-dowork state machine jump to the datafile getattr
     * operation state. 
     */
    if(missing_attrs == PVFS_ATTR_DATA_SIZE)
    {
        if(!(sm_p->getattr.attr.mask & PVFS_ATTR_META_UNSTUFFED))
        {
            /* We are missing the size, and we don't know for sure if the
             * file has been unstuffed.  In this case, act as though we
             * missed on all atributes so that we can get fresh stuffed size
             * or datafile information as needed.
             */
        }
        else
        {
            /* if the file size is requested but the distribution info
             * isn't and it hasn't been cached, then we need to
             * get that first.
             */
            PINT_SM_DATAFILE_SIZE_ARRAY_INIT(
                &sm_p->getattr.size_array,
                sm_p->getattr.attr.u.meta.dfile_count);

            js_p->error_code = GETATTR_NEED_DATAFILE_SIZES;
            gossip_debug(GOSSIP_ACACHE_DEBUG, "acache: acache hit, need sizes"
                         "[%llu]\n", llu(object_ref.handle));
            return SM_ACTION_COMPLETE;
        }
    }

    /* we missed */
    /* clean out the attributes we got from the cache; this will be
     * overwritten when we request updated information from the server
     */
    PINT_free_object_attr(&sm_p->getattr.attr);
    gossip_debug(GOSSIP_ACACHE_DEBUG, "acache: acache miss due to mask: "
        " [%llu]\n",
          llu(object_ref.handle));

    js_p->error_code = GETATTR_ACACHE_MISS;
    return SM_ACTION_COMPLETE;
}


static PINT_sm_action getattr_object_getattr_setup_msgpair(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret = -PVFS_EINVAL;
    PVFS_object_ref object_ref;
    PINT_sm_msgpair_state *msg_p;

    gossip_debug(GOSSIP_CLIENT_DEBUG, "(%p) %s\n", sm_p, __func__);

    js_p->error_code = 0;

    PINT_msgpair_init(&sm_p->msgarray_op);
    msg_p = &sm_p->msgarray_op.msgpair;

    object_ref = sm_p->getattr.object_ref;

    assert(object_ref.fs_id != PVFS_FS_ID_NULL);
    assert(object_ref.handle != PVFS_HANDLE_NULL);

    /* setup the msgpair to do a getattr operation */
    PINT_SERVREQ_GETATTR_FILL(
        msg_p->req,
        *sm_p->cred_p,
        object_ref.fs_id,
        object_ref.handle,
        sm_p->getattr.req_attrmask,
        sm_p->hints);
    
    msg_p->fs_id = object_ref.fs_id;
    msg_p->handle = object_ref.handle;
    msg_p->retry_flag = PVFS_MSGPAIR_RETRY;
    msg_p->comp_fn = getattr_object_getattr_comp_fn;

    ret = PINT_cached_config_map_to_server(
        &msg_p->svr_addr, msg_p->handle,
        msg_p->fs_id);
    if (ret)
    {
        gossip_err("Failed to map meta server address\n");
        js_p->error_code = ret;
    }

    PINT_sm_push_frame(smcb, 0, &sm_p->msgarray_op);
    return SM_ACTION_COMPLETE;
}

/*
  copies data from getattr response into the user supplied sys_attr
  structure.  returns 0 for directories and symlinks, and
  GETATTR_NEED_DATAFILE_SIZES for a metafile (when appropriate)
*/
static int getattr_object_getattr_comp_fn(
    void *v_p,
    struct PVFS_server_resp *resp_p,
    int index)
{
    PVFS_object_attr *attr = NULL;
    PINT_smcb *smcb = v_p;
    PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_MSGPAIR_PARENT_SM);

    assert(resp_p->op == PVFS_SERV_GETATTR);

    gossip_debug(GOSSIP_GETATTR_DEBUG,
                 "getattr_object_getattr_comp_fn called\n");

    if (resp_p->status != 0)
    {
        return resp_p->status;
    }

    /*
     * If we've reached the callback for the getattr msgpair tranfer,
     * then we can make a copy of the retrieved attribute for later
     * caching.
     */
    PINT_copy_object_attr(&sm_p->getattr.attr,
                          &resp_p->u.getattr.attr);

    attr =  &sm_p->getattr.attr;

    /* if the ref_type mask is set to a non-zero value (!PVFS_TYPE_NONE)
     * a -PVFS_error will be triggered if the
     * attributes received are not one of the the types specified.
     * This is useful so that the client can know (in some cases) that it
     * can avoid issuing an operation to the server since the server will 
     * just pass an error back anyway.
     */
    if(sm_p->getattr.ref_type &&
       sm_p->getattr.ref_type != attr->objtype)
    {
        int ret;
        gossip_debug(GOSSIP_CLIENT_DEBUG, "*** "
                     "getattr_comp_fn: Object type mismatch.\n Possibly "
                     "saving network roundtrip by returning an error\n");

        if (sm_p->getattr.ref_type == PVFS_TYPE_DIRECTORY)
        {
            ret = -PVFS_ENOTDIR;
        }
        else
        {
            assert(sm_p->getattr.ref_type == PVFS_TYPE_METAFILE);
            ret = ((attr->objtype == PVFS_TYPE_DIRECTORY) ?
                   -PVFS_EISDIR : -PVFS_EBADF);
        }
        PVFS_perror_gossip("Object Type mismatch error", ret);
        return ret;
    }

    /* do assertion checking of getattr response values, and
     * check if file sizes are needed.  With NDEBUG defined, this block
     * only checks if file sizes are needed.
     */
    switch (attr->objtype)
    {
        case PVFS_TYPE_METAFILE:
            if (sm_p->msgarray_op.msgpair.req.u.getattr.attrmask &
                PVFS_ATTR_META_DIST)
            {
                /* if we requested distribution attrs, did the distribution 
                 * get set and is the size valid?
                 */
                assert(attr->mask & PVFS_ATTR_META_DIST);
                assert(attr->u.meta.dist && (attr->u.meta.dist_size > 0));
            }

            if (sm_p->msgarray_op.msgpair.req.u.getattr.attrmask &
                PVFS_ATTR_META_DFILES)
            {
                /* if we requested the datafile handles for the file, did
                 * the datafile array get populated?
                 */
                assert(attr->u.meta.dfile_array &&
                       (attr->u.meta.dfile_count > 0));

                gossip_debug(GOSSIP_GETATTR_DEBUG,
                             "getattr_object_getattr_comp_fn: "
                             "%d datafiles.\n", attr->u.meta.dfile_count);
                
                /* if we need the size, that should be the only time we're 
                 * going to have to do a full data file fetch. 
                 * (that's expensive)
                 */
                if (sm_p->getattr.req_attrmask & PVFS_ATTR_DATA_SIZE)
                {
                    /* is the file stuffed? */
                    if(!(attr->mask & PVFS_ATTR_META_UNSTUFFED))
                    {
                        /* we can compute the size without doing any more
                         * getattr requests
                         */
                        gossip_debug(GOSSIP_GETATTR_DEBUG,
                            "getattr_object_getattr_comp_fn: detected stuffed file.\n");
                        return(0);
                    }
                    /* if caller asked for the size, then we need
                     * to jump to the datafile_getattr state, which
                     * will retrieve the datafile sizes for us.
                     */
                    PINT_SM_DATAFILE_SIZE_ARRAY_INIT(
                        &sm_p->getattr.size_array,
                        attr->u.meta.dfile_count);
                    return GETATTR_NEED_DATAFILE_SIZES;
                }
            }
            return 0;
        case PVFS_TYPE_DIRECTORY:
        {
            gossip_debug(GOSSIP_CLIENT_DEBUG,
                "getattr comp_fn [%p] "
                "dfile_count = %d "
                "dist_name_len = %d "
                "dist_params_len = %d\n",
                attr,
                attr->u.dir.hint.dfile_count,
                attr->u.dir.hint.dist_name_len,
                attr->u.dir.hint.dist_params_len);
            return 0;
        }
        case PVFS_TYPE_SYMLINK:
            return 0;
        case PVFS_TYPE_DATAFILE:
	    return 0;
        case PVFS_TYPE_DIRDATA:
	    return 0;
        case PVFS_TYPE_INTERNAL:
	    return 0;
        default:
            gossip_err("error: getattr_object_getattr_comp_fn: "
                       "handle refers to invalid object type\n");
    }
    return -PVFS_EINVAL;
}


static PINT_sm_action getattr_object_getattr_failure(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p __attribute__((unused)) = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    gossip_debug(
        GOSSIP_CLIENT_DEBUG,
        "(%p) getattr state: getattr_object_getattr_failure\n", sm_p);

    if ((js_p->error_code != -PVFS_ENOENT) &&
        (js_p->error_code != -PVFS_EINVAL))
    {
        PVFS_perror_gossip("getattr_object_getattr_failure ",
                           js_p->error_code);
    }

    return SM_ACTION_COMPLETE;
}

static PINT_sm_action getattr_datafile_getattr_setup_msgpairarray(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret = -PVFS_EINVAL;
    PVFS_object_attr *attr = NULL;
    PINT_sm_msgpair_state *msg_p;
    int index;

    js_p->error_code = 0;

    attr = &sm_p->getattr.attr;
    assert(attr);

    ret = PINT_msgpairarray_init(&sm_p->msgarray_op, attr->u.meta.dfile_count);
    if(ret != 0)
    {
        gossip_err("Failed to initialize %d msgpairs\n", attr->u.meta.dfile_count);
        js_p->error_code = ret;
        return SM_ACTION_COMPLETE;
    }

    /* for each datafile, post a send/recv pair to obtain the size */
    foreach_msgpair(&sm_p->msgarray_op, msg_p, index)
    {
        assert(msg_p);

        gossip_debug(GOSSIP_GETATTR_DEBUG,
                     "  datafile_getattr: getting size for handle %llu\n",
                     llu(attr->u.meta.dfile_array[index]));

        PINT_SERVREQ_GETATTR_FILL(
            msg_p->req,
            *sm_p->cred_p,
            sm_p->getattr.object_ref.fs_id,
            attr->u.meta.dfile_array[index],
            PVFS_ATTR_DATA_SIZE,
            sm_p->hints);

        msg_p->fs_id = sm_p->getattr.object_ref.fs_id;
        msg_p->handle = attr->u.meta.dfile_array[index];
        msg_p->retry_flag = PVFS_MSGPAIR_RETRY;
        msg_p->comp_fn = getattr_datafile_getattr_comp_fn;
    }

    ret = PINT_serv_msgpairarray_resolve_addrs(&sm_p->msgarray_op);
    if (ret < 0)
    {
        gossip_lerr("Error: failed to resolve meta server addresses.\n");
        js_p->error_code = ret;
    }

    PINT_sm_push_frame(smcb, 0, &sm_p->msgarray_op);
    return SM_ACTION_COMPLETE;
}

static int getattr_datafile_getattr_comp_fn(
    void *v_p, struct PVFS_server_resp *resp_p, int index)
{
    PINT_smcb *smcb = v_p;
    PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_MSGPAIR_PARENT_SM);

    if (resp_p->status != 0)
    {
        return resp_p->status;
    }

    assert(resp_p->op == PVFS_SERV_GETATTR);

    gossip_debug(GOSSIP_GETATTR_DEBUG,
                 "datafile_getattr: size of datafile %d is %lld\n",
                 index, lld(resp_p->u.getattr.attr.u.data.size));

    sm_p->getattr.size_array[index] =
        resp_p->u.getattr.attr.u.data.size;

    return 0;
}

static PINT_sm_action getattr_datafile_getattr_failure(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p __attribute__((unused)) = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    gossip_debug(GOSSIP_CLIENT_DEBUG, "(%p) getattr state: "
                 "getattr_datafile_getattr_failure\n", sm_p);
    return SM_ACTION_COMPLETE;
}

static PINT_sm_action getattr_datafile_getattr_cleanup(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    if(js_p->error_code == 0)
    {
        PINT_msgpairarray_destroy(&sm_p->msgarray_op);
    }

    return SM_ACTION_COMPLETE;
}

static PINT_sm_action getattr_acache_insert(
        struct PINT_smcb *smcb, job_status_s *js_p)
{   
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    PVFS_size* tmp_size = NULL;

    if( sm_p->getattr.attr.objtype == PVFS_TYPE_METAFILE || 
        sm_p->getattr.attr.objtype == PVFS_TYPE_DIRECTORY ||
        sm_p->getattr.attr.objtype == PVFS_TYPE_SYMLINK )
    {
        /* see if we have a size value to cache */
        if (sm_p->getattr.attr.objtype == PVFS_TYPE_METAFILE &&
            sm_p->getattr.req_attrmask & PVFS_ATTR_DATA_SIZE)
        {                                           
            if(!(sm_p->getattr.attr.mask & PVFS_ATTR_META_UNSTUFFED))
            {
                /* stuffed file case */
                sm_p->getattr.size = sm_p->getattr.attr.u.meta.stuffed_size;
                tmp_size = &sm_p->getattr.size;
                gossip_debug(GOSSIP_GETATTR_DEBUG, "getattr_acache_insert calculated stuffed logical size of %lld\n", lld(*tmp_size));
            }
            else
            {
                /* compute size as requested */
                assert(sm_p->getattr.attr.u.meta.dist);
                assert(sm_p->getattr.attr.u.meta.dist->methods &&
                       sm_p->getattr.attr.u.meta.dist->methods->logical_file_size);
                
                sm_p->getattr.size =
                    (sm_p->getattr.attr.u.meta.dist->methods->logical_file_size)(
                    sm_p->getattr.attr.u.meta.dist->params,
                    sm_p->getattr.attr.u.meta.dfile_count,
                    sm_p->getattr.size_array);

                tmp_size = &sm_p->getattr.size;
                gossip_debug(GOSSIP_GETATTR_DEBUG, "getattr_acache_insert calculated unstuffed logical size of %lld\n", lld(*tmp_size));
            }
        }

        PINT_acache_update(sm_p->getattr.object_ref,
            &sm_p->getattr.attr,
            tmp_size);

        gossip_debug(GOSSIP_CLIENT_DEBUG, "trying to add object "
                     "reference to acache\n");
    }

    return SM_ACTION_COMPLETE;
}

static PINT_sm_action getattr_cleanup(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    gossip_debug(GOSSIP_CLIENT_DEBUG,
                 "(%p) getattr state: getattr_cleanup\n", sm_p);

    sm_p->error_code = js_p->error_code;

    PINT_SM_DATAFILE_SIZE_ARRAY_DESTROY(&sm_p->getattr.size_array);

    return SM_ACTION_COMPLETE;
}

static PINT_sm_action getattr_set_sys_response(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    PVFS_sysresp_getattr * sysresp = NULL;
    PVFS_object_attr *attr = NULL;

    if(js_p->error_code != 0)
    {
        PINT_SET_OP_COMPLETE;
        return SM_ACTION_TERMINATE;
    }

    attr = &sm_p->getattr.attr;
    assert(attr);
    
    /* If we get to this state action, 
     * the getattr state machine was invoked, so
     * we can assume that one of the PVFS_[i]sys_getattr functions
     * was called, and the response field must be filled in for the
     * user.
     */

    sysresp = sm_p->u.getattr.getattr_resp_p;
    
    /*
     * if we retrieved a symlink target, copy it for the caller; this
     * target path will be handed all the way back up to the caller via
     * the PVFS_sys_attr object.  The caller of PVFS_[i]sys_getattr
     * must free it.
     */
    if(attr->objtype == PVFS_TYPE_SYMLINK &&
       attr->mask & PVFS_ATTR_SYMLNK_TARGET)
    {
        assert(attr->u.sym.target_path_len > 0);
        assert(attr->u.sym.target_path);

        sysresp->attr.link_target = strdup(attr->u.sym.target_path);
        if (!sysresp->attr.link_target)
        {
           js_p->error_code = -PVFS_ENOMEM;
           PINT_SET_OP_COMPLETE;
           return SM_ACTION_TERMINATE;
        }
    }

    if(attr->objtype == PVFS_TYPE_METAFILE) 
    {
       /* Copy if there are any special object specific flags */
       sysresp->attr.flags = attr->u.meta.hint.flags;
       /* special case for when users ask for dfile count */
       if (sm_p->getattr.req_attrmask & PVFS_ATTR_META_DFILES)
       {
           sysresp->attr.dfile_count = attr->u.meta.dfile_count;
       }
    }
    if (attr->objtype == PVFS_TYPE_DIRECTORY)
    {
        gossip_debug(GOSSIP_CLIENT_DEBUG, "dfile_count: %d\n", 
            attr->u.dir.hint.dfile_count);
        gossip_debug(GOSSIP_CLIENT_DEBUG, "dist_name_len = %d, dist_params_len = %d\n",
            attr->u.dir.hint.dist_name_len, attr->u.dir.hint.dist_params_len);
        sysresp->attr.dfile_count = attr->u.dir.hint.dfile_count;
        /* 
         * If we retrieved any extended attributes for the directory
         * in question, the caller's responsibility to free it up
         */
        if (attr->u.dir.hint.dist_name_len > 0 && 
            (sm_p->getattr.req_attrmask & PVFS_ATTR_DIR_HINT))
        {
            sysresp->attr.dist_name = strdup(attr->u.dir.hint.dist_name);
            if (!sysresp->attr.dist_name)
            {
                js_p->error_code = -PVFS_ENOMEM;
                PINT_SET_OP_COMPLETE;
                return SM_ACTION_TERMINATE;
            }
            gossip_debug(GOSSIP_CLIENT_DEBUG, "dist_name_hint: %s\n", sysresp->attr.dist_name);
        }
        if (attr->u.dir.hint.dist_params_len > 0 &&
            (sm_p->getattr.req_attrmask & PVFS_ATTR_DIR_HINT))
        {
            sysresp->attr.dist_params = strdup(attr->u.dir.hint.dist_params);
            if (!sysresp->attr.dist_params)
            {
                free(sysresp->attr.dist_name);
                sysresp->attr.dist_name = NULL;
                js_p->error_code = -PVFS_ENOMEM;
                PINT_SET_OP_COMPLETE;
                return SM_ACTION_TERMINATE;
            }
            gossip_debug(GOSSIP_CLIENT_DEBUG, "dist_name_params: %s\n", sysresp->attr.dist_params);
        }
    }

    /* copy outgoing sys_attr fields from returned object_attr */
    sysresp->attr.owner = attr->owner;
    sysresp->attr.group = attr->group;
    sysresp->attr.perms = attr->perms;
    sysresp->attr.atime = attr->atime;
    sysresp->attr.mtime = attr->mtime;
    sysresp->attr.ctime = attr->ctime;
    sysresp->attr.mask  = PVFS_util_object_to_sys_attr_mask(attr->mask);
    sysresp->attr.size  = 0;
    sysresp->attr.objtype = attr->objtype;

    if (js_p->error_code == 0)
    {
        /* convert outgoing attribute mask based on what we got */
        sysresp->attr.mask = PVFS_util_object_to_sys_attr_mask(
            sm_p->getattr.attr.mask);

       if (sm_p->getattr.req_attrmask & PVFS_ATTR_DATA_SIZE)
        {
            if( attr->objtype == PVFS_TYPE_DATAFILE )
            {
                sysresp->attr.size = attr->u.data.size;
            }
            else
            {
                sysresp->attr.size = sm_p->getattr.size;
            }

            sysresp->attr.mask |= PVFS_ATTR_SYS_SIZE;
        }

        if(attr->mask & PVFS_ATTR_META_DIST)
        {
            /* we have enough information to set a block size */
            sysresp->attr.blksize = attr->u.meta.dist->methods->get_blksize(
                attr->u.meta.dist->params);
            sysresp->attr.mask |= PVFS_ATTR_SYS_BLKSIZE;
        }

        /* if this is a symlink, add the link target */
        if (sm_p->getattr.req_attrmask & PVFS_ATTR_SYMLNK_TARGET)
        {
            sysresp->attr.mask |= PVFS_ATTR_SYS_LNK_TARGET;
        }

        if(sm_p->getattr.req_attrmask & PVFS_ATTR_DIR_DIRENT_COUNT)
        {
            sysresp->attr.dirent_count = attr->u.dir.dirent_count;
            sysresp->attr.mask |= PVFS_ATTR_SYS_DIRENT_COUNT;
        }
    }
    else
    {
        /* in case of failure, blank out response */ 
        memset(sm_p->u.getattr.getattr_resp_p,
               0, sizeof(PVFS_sysresp_getattr));
    }

    PINT_SM_GETATTR_STATE_CLEAR(sm_p->getattr);

    PINT_SET_OP_COMPLETE;
    return SM_ACTION_TERMINATE;
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

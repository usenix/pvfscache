/* this file includes a couple functions to initiate all kinds resources and free them
 * when needed. */

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "ncac-list.h"
#include "wormup-cache.h"
#include "radix.h"


static int cache_initialized = 0;

static inline void init_free_extent_list(int num);
static inline void init_free_req_list(int num);
static inline void init_cache_stack_list(void);

unsigned long wormup_radix_get_value(const void *item);

/* cache_init(): initiate cache. */
int cache_init()
{
    if ( !cache_initialized ) {
		dev.cachesize   = WORMUP_CACHE_SIZE*1048576;
		dev.extsize     = WORMUP_EXTENT_SIZE;
		dev.cachemem = malloc(info.cachesize);
        if (!info.cachespace) {
			fprintf(stderr, "cannot allocate memory for the cache\n");
			return(-PVFS_ENOMEM);
		}

        /* we expect extcnt is a power of two number */
        dev.extcnt = (dev.cachesize/dev.extsize);

        dev.free_extent_src = (struct wormup_cache_extent *)malloc(dev.extcnt*sizeof(struct wormup_cache_extent) );
        memset(dev.free_extent_src, 0, dev.extcnt*sizeof(struct wormup_cache_extent) );
        if ( dev.free_extent_src == NULL){
            fprintf(stderr, "cache_init: cannot allocate extent memory\n");
            return -ENOMEM;
        }

        init_free_extent_list(dev.extcnt);
        init_cache_stack_list();


        memset( inode_arr, 0, sizeof(struct inode*)*MAX_INODE_NUM );

        dev.get_value = radix_get_value;
        dev.max_b     = RADIX_MAX_BITS;
		cache_initialized = 1;
    }
    return 0;
}


/* for radix tree if linux radix tree is not used */
unsigned long wormup_radix_get_value(const void *item)
{
    const struct wormup_cache_extent *e = item;
    return e->index;
}



static inline void init_free_extent_list(int num)
{
    int i;
    struct wormup_cache_extent *start;
    struct wormup_cache_stack *cache;
    struct list_head *head;

    cache = &dev.wormup_cache_stack;
    start = dev.free_extent_src;


    head = &cache->free_extent_list;
    INIT_LIST_HEAD( head );

    for (i = 0; i < num; i++ ){
        list_add_tail( &start[i].list, head );
        start[i].addr = dev.cachemem+i*dev.extsize;
        start[i].ioreq = -1;
    }
    cache->nr_free = num;
    cache->nr_dirty = 0;
}

static inline void init_cache_stack_list()
{
    struct wormup_cache_stack *cache;

    cache = &dev.wormup_cache_stack;

    cache->nr_active = cache->nr_inactive = 0;
    INIT_LIST_HEAD( &cache->active_list);
    INIT_LIST_HEAD( &cache->inactive_list);
	//spin_lock_init(&cache->lock);
}

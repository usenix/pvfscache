#ifndef __WORMUP_CACHE_H
#define __WORMUP_CACHE_H

#include <pthread.h>

#include "radix.h"
#include "ncac-list.h"

/* Cache Size in MB: 200MB */
#define WORMUP_CACHE_SIZE 200
/* Cache extent size: 64KB */
#define WORMUP_EXTENT_SIZE 65536


#define DISCARD_CLUSTER_MAX 32
#define REFILL_CLUSTER_MAX  32

#define DISCARD_CLUSTER_MIN 4
#define DELT_DISCARD_NUM    5

#define LRU_POLICY      1
#define ARC_POLICY      2
#define TWOQ_POLICY       3

#define WORMUP_MAX_INODE_NUM 10000


/* global variable */
WORMUP_dev dev;
//Store opened inode
struct wormup_cache_inode *wormup_inode_arr[WORMUP_MAX_INODE_NUM];


typedef uint64_t file_handle;



struct WORMUP_dev{
    int cachesize;
    int extsize;
    void *cachemem;
    int extcnt;
    struct wormup_cache_extent *free_extent_src;
    stuct wormup_cache_stack wormup_cache_stack;
    /* for radix tree if LINUX radix tree is not used */
    unsigned long (* get_value)(const void *);
    int max_b;
};


struct wormup_cache_stack {
    struct list_head list;
    //pthread_mutex_t lock;
    unsigned long    nr_inactive;
    unsigned long    nr_active;
    struct list_head active_list;
    struct list_head inactive_list;

    unsigned long 	 nr_free;
    struct list_head free_extent_list;
	unsigned long    nr_dirty;
	unsigned long    nr_writeback;
	unsigned long    ratelimits;
};


struct wormup_cache_inode
{
    file_handle      handle;

    struct radix_tree_root page_tree;

    struct list_head clean_pages;
    struct list_head dirty_pages;
    unsigned long nrpages;
    int  nr_dirty;

    struct wormup_cache_stack *wormup_cache_stack;

	struct wormup_cache_inode *next;
};


struct wormup_cache_extent {
   //unsigned long   flags;
   int 		status;
   char 	*addr;
   int  	id;
   unsigned long index;

   struct list_head  list;
   struct list_head  lru;

   unsigned int writes;
   unsigned int reads;
   unsigned int rcmp;
   unsigned int wcmp;

   struct wormup_cache_extent *next;
   struct wormup_cache_inode *mapping;

   struct wormup_cache_extent    *ioreq_next;
};




struct wormup_cache_extent *wormup_lookup_cache_item(struct wormup_cache_inode *mapping, unsigned long index);
struct wormup_cache_extent *get_free_extent_list_item(struct list_head *list);
int wormup_add_cache_item(struct wormup_cache_extent *page, struct wormup_cache_inode *mapping,
                    unsigned long index, int policy);

void wormup_remove_cache_item(struct wormup_cache_extent *page, int policy);

int wormup_shrink_cache(struct wormup_cache_stack *cache_stack, unsigned int expected,
                    int policy, unsigned int *shrinked);

int wormup_is_extent_discardable(struct wormup_cache_extent *victim);

void wormup_hit_cache_item(struct wormup_cache_extent *page, int policy);

#endif

/* common functions for cache management. These functions are shared
 * by all cache policies.
 */

#include <unistd.h>
#include <string.h>

#include "wormup-cache.h"
#include "wormup-io.h"
//#include "wormup-locks.h"



static inline struct wormup_cache_extent *wormup_allocate_extent(wormup_cache_inode *mapping, int flag)
{
    struct extent *new = NULL;
	struct wormup_cache_stack *cache;
    unsigned int shrinked;

    char *buf;
    int ret;

	cache = mapping->wormup_cache_stack;

    if ( !list_empty( &cache->free_extent_list ) ) {
		//cache_lock( &cache->lock);
        new = wormup_get_free_extent_list_item( &(cache->free_extent_list) );
	    //cache_unlock(&cache->lock);

        if ( new ) {
    		buf = new->addr;
   			memset(new, 0, sizeof(struct wormup_cache_extent));
    		new->addr = buf;
    		//SetPageBlank(new);
    		//DPRINT("new extent:%p, flags:%lx\n", new, new->flags);
			return new;
		}
    }

    /* No free extent so far */
    if ( BLOCKING_EXTENT_ALLOC == flag ){

		//cache_lock( &cache->lock);
        ret = wormup_shrink_cache(cache, DELT_DISCARD_NUM, LRU_POLICY, &shrinked);
        if ( ret < 0 ) {
            ncac_req->error = ret;
		    cache_unlock(&cache->lock);
            return NULL;
        }
        new = wormup_get_free_extent_list_item( &(mapping->wormup_cache_stack->free_extent_list) );
	    //cache_unlock(&cache->lock);

	    if ( !new ) return NULL;
        else {
    		buf = new->addr;
   			memset(new, 0, sizeof(struct extent));
    		new->addr = buf;
    		//SetPageBlank(new);
    		//DPRINT("new extent:%p, flags:%lx\n", new, new->flags);
			return new;
	    }
    }

    return NULL;
}



int wormup_read(int fildes, void *buf, int nbytes, int offset)
{
    int index, end_index;
    char *tmp = buf;
    struct wormup_cache_extent *avail_extent;
    struct wormup_cache_inode *mapping;

    /* get the inode */
    struct stat statbuf;
    fstat(fildes, &statbuf);
    unsigned long long handle = statbuf.st_ino;

    mapping = search_wormup_inode_list(handle);
    index = offset >> dev.extlog2;
    end_index = (offset + nbytes - 1) >> dev.extlog2;
    boffset = offset - (index * WORMUP_EXTENT_SIZE);

    for (int i = index; i <= end_index; ++i)
    {
        avail_extent = wormup_lookup_cache_item(mapping, i);
        if (NULL == avail_extent){
            new_extent= allocate_extent(ncac_req,BLOCKING_EXTENT_ALLOC);
            if ( new_extent ){
                new_extent->index = i;
                new_extent->mapping = mapping;
                //pread
                ret = pread(mapping->handle, new_extent->addr,
                      WORMUP_EXTENT_SIZE, i*WORMUP_EXTENT_SIZE);
                if (i != end_index)
                    memcpy(tmp, new_extent->addr + boffset, WORMUP_EXTENT_SIZE - boffset);
                    tmp += WORMUP_EXTENT_SIZE - boffset;
                    boffset = 0;
                else
                    memcpy(tmp, new_extent->addr + boffset, ((offset + nbytes) - end_index * WORMUP_EXTENT_SIZE));
                /*
                ret = init_extent_read(ncac_req, new_extent,
                                foff[i], dev.extsize);
                */
                if ( ret < 0 ) {
                    return -1;
                }
                wormup_add_cache_item(new_extent, mapping, i, LRU_POLICY);
            }

        }else{ /* cached */
            cbufhash[i] = new_extent;
            wormup_hit_cache_item(new_extent, LRU_POLICY);
        }/* end if */
    }/* end for */
    return nbytes;
}




static inline struct wormup_cache_inode *search_wormup_inode_list (file_handle handle)
{
	int inode_index;
	struct inode * cur;

	inode_index = handle % WORMUP_MAX_INODE_NUM;

	cur = wormup_inode_arr[inode_index];
	while ( NULL != cur ) {
		if ( cur->handle == handle ) return cur;
		cur = cur->next;
	}

	return NULL;
}


static inline struct wormup_cache_inode *get_wormup_inode(file_handle handle)
{
	struct wormup_cache_inode *inode;
	int inode_index;

	inode_index = handle % MAX_INODE_NUM;
	inode = search_wormup_inode_list (handle);

    gossip_debug(GOSSIP_WORMUP_DEBUG, "handle: %lld, index: %d\n",
                 lld(handle), inode_index);

	if ( NULL == inode ){
		inode=(struct wormup_cache_inode*)malloc(sizeof(struct wormup_cache_inode));

		/* initialize it */
		memset(inode, 0, sizeof(struct wormup_cache_inode));
        inode->handle = handle;
        inode->cache_stack = get_cache_stack();
		inode->nrpages = 0;
		inode->nr_dirty = 0;


		init_single_radix_tree(&inode->page_tree, wormup_radix_get_value, max_b);

		INIT_LIST_HEAD(&(inode->clean_pages));
		INIT_LIST_HEAD(&(inode->dirty_pages));

		/* put the new inode to the head of the collision list */
		inode->next = inode_arr[inode_index];
		inode_arr[inode_index] = inode;
	}

	return inode;
}

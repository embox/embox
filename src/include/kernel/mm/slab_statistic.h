/**
 * @file
 * @brief Define functions for statistic for slab allocator
 *
 * @date 14.12.2010
 * @author Alexandr Kalmuk
 */

#ifndef SLAB_STATISTIC_H
#define SLAB_STATISTIC_H

/**
 * Return list of free and busy blocks per current slab
 * @param list of free and busy blocks(for objects)
 * @param slabp is pointer to slab, in witch blocks are
 */
extern void sget_blocks_info(struct list_head* list, struct list_head *slabp);

/**
 *
 * @param list is list_head struct:
 * it is'nt a doubly linked list
 * _____________
 *|pseudo_list  |
 *|_____________|____________prev_____________
 *     |next                                 |
 * ____|________       ______               _|____
 *|kmalloc_cache|<--->|cache1|<--->...<--->|cacheN|
 *|_____________|     |______|             |______|
 */
extern void make_caches_list(struct list_head* list);

#endif /* SLAB_STATISTIC_H */

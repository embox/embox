/**
 * @file
 * @brief

 * @date 07.04.2014
 * @author Alexander Kalmuk
 */

#ifndef MSPACE_MALLOC_H_
#define MSPACE_MALLOC_H_

#include <defines/size_t.h>
#include <util/dlist.h>

extern void *mspace_memalign(size_t boundary, size_t size, struct dlist_head *mspace);
extern void *mspace_malloc(size_t size, struct dlist_head *mspace);
extern int   mspace_free(void *ptr, struct dlist_head *mspace);
extern void *mspace_calloc(size_t nmemb, size_t size, struct dlist_head *mspace);
extern void *mspace_realloc(void *ptr, size_t size, struct dlist_head *mspace);

#endif /* MSPACE_MALLOC_H_ */

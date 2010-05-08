/**
 * @file
 * @brief multipage allocator header
 * @details
 *
 * @date 28.04.10
 * @author Fedor Burdun
 */

#ifndef __MULTIPAGE_ALLOC_H_
#define __MULTIPAGE_ALLOC_H_

#ifndef EXTENDED_TEST
# include <types.h>
#else
# include <bits/types.h>
# include <unistd.h>
# include <cdio/types.h>
#endif

void* multipage_alloc(size_t page_quantity);
void  multipage_free (void * pointer);

#endif /* __MULTIPAGE_ALLOC_H_ */


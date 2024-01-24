/**
 * @file
 *
 * @date Aug 21, 2014
 * @author: Anton Bondarev
 */

#ifndef MAREA_H_
#define MAREA_H_

#include <stdint.h>
#include <lib/libds/dlist.h>

struct marea {
	uintptr_t start;
	size_t size;
	int flags;

	struct dlist_head mmap_link;
};

extern struct marea *marea_alloc(uintptr_t start, size_t size, int flags);
extern void marea_free(struct marea *marea);

#endif /* MAREA_H_ */

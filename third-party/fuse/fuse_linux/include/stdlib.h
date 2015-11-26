/**
 * @file
 *
 * @data 26.11.2015
 * @author: Anton Bondarev
 */

#ifndef THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_STDLIB_H_
#define THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_STDLIB_H_

#include_next <stdlib.h>

static int posix_memalign(void **memptr, size_t alignment, size_t size) {
	*memptr = memalign(alignment, size);
	return 0;
}


#undef HAVE_SPLICE
#undef HAVE_VMSPLICE

#endif /* THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_STDLIB_H_ */

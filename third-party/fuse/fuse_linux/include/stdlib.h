/**
 * @file
 *
 * @date 26.11.2015
 * @author: Anton Bondarev
 */

#ifndef THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_STDLIB_H_
#define THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_STDLIB_H_

#include_next <stdlib.h>

#undef HAVE_SPLICE
#undef HAVE_VMSPLICE

#endif /* THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_STDLIB_H_ */

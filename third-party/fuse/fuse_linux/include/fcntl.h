/**
 * @file
 *
 * @date 26.11.2015
 * @author: Anton Bondarev
 */

#ifndef THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_FCNTL_H_
#define THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_FCNTL_H_

#include_next <fcntl.h>

#define AT_SYMLINK_NOFOLLOW 0x10

#undef HAVE_SPLICE
#undef HAVE_VMSPLICE



#endif /* THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_FCNTL_H_ */

/**
 * @file
 *
 * @date 25.1.2015
 * @author: Anton Bondarev
 */

#ifndef THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_ERRNO_H_
#define THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_ERRNO_H_

#include_next <errno.h>
#define ESTALE 0x101
#define EPROTO 0x102

#endif /* THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_ERRNO_H_ */

/**
 * @file
 *
 * @date Sep 9, 2021
 * @author Anton Bondarev
 */

#ifndef THIRD_PARTY_LIB_NPF_EMBOX_COMPAT_H_
#define THIRD_PARTY_LIB_NPF_EMBOX_COMPAT_H_

#include <errno.h>

#define EPROGMISMATCH   ENOSUPP

#define __KERNEL_RCSID(x,y)
#define __linux__


#include <nv.h>

typedef nvlist_t * nvlist_ref_t;

#define MAP_FILE   0

#ifndef __UNCONST
#define	__UNCONST(a)		((void*)(uintptr_t)(const void*)a)
#endif

#define __unused

static inline int nvlist_xfer_ioctl(int fd, unsigned long cmd, nvlist_t *req, nvlist_t **resp) {
	return -1;
}

#endif /* THIRD_PARTY_LIB_NPF_EMBOX_COMPAT_H_ */

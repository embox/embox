/*
 * @file
 *
 * @date Jan 17, 2013
 * @author: Anton Bondarev
 */

#ifndef POSIX_UIO_H_
#define POSIX_UIO_H_

#include <stddef.h>

struct iovec {
	void   *iov_base; /**<Base address of a memory region for input or output*/
	size_t  iov_len;  /**< The size of the memory pointed to by iov_base. */
};

#endif /* POSIX_UIO_H_ */

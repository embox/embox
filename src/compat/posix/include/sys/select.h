/**
 * @file
 * @brief Synchronous I/O multiplexing
 *
 * @date 12.09.11
 * @author Anton Bondarev
 */

#ifndef SYS_SELECT_H_
#define SYS_SELECT_H_

#include <sys/types.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

#define _FDSETWORDS       0x10
#define _FDSETBITSPERWORD 0x20 /* 32 bits */

#define _FD_BITMASK(b) 	   (1L << ((b) % _FDSETBITSPERWORD))
#define _FD_BITWORD(b)     ((b)/_FDSETBITSPERWORD)

/* The fd_set member is required to be an array of longs.  */
typedef long int __fd_mask;

typedef struct {
	__fd_mask fds_bits[_FDSETWORDS];
} fd_set;

#define FD_SETSIZE (_FDSETWORDS * LONG_BIT)

/** Clear a set */
#define FD_ZERO(s) \
	do {                                          \
		int _i;                               \
		for(_i = 0; _i < _FDSETWORDS; _i++) { \
			(s)->fds_bits[_i] = 0;        \
		}                                     \
	} while(0)

/** Add a given file descriptor to a set */
#define FD_SET(f, s) \
	do {                                                     \
		(s)->fds_bits[_FD_BITWORD(f)] |= _FD_BITMASK(f); \
	} while(0)

/* Tests to see if a file descriptor is part of the set */
#define FD_ISSET(f, s) \
	((s)->fds_bits[_FD_BITWORD(f)] & _FD_BITMASK(f))

/**  Remove a given file descriptor from a set */
#define FD_CLR(f, s) \
	do {                                                        \
		(s)->fds_bits[_FD_BITWORD(f)] &= (~_FD_BITMASK(f)); \
	} while(0)

struct timeval;

extern int select(int nfds, fd_set *readfds, fd_set *writefds,
			fd_set *exceptfds, struct timeval *timeout);

__END_DECLS

#endif /* SYS_SELECT_H_ */

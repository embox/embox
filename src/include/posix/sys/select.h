/**
 * @file
 *
 * @brief
 *
 * @date 12.09.2011
 * @author Anton Bondarev
 */

#ifndef SYS_SELECT_H_
#define SYS_SELECT_H_

/* The fd_set member is required to be an array of longs.  */
typedef long int __fd_mask;


typedef struct {
	__fd_mask fds_bits[10];
} fd_set;

#endif /* SYS_SELECT_H_ */

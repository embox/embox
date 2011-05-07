/**
 * @file
 * @brief Describes asynchronous I/O operations
 *
 * @date 13.01.10
 * @author Anton Bondarev
 */

#ifndef LINUX_AIO_H_
#define LINUX_AIO_H_

#include <lib/list.h>
/**
 * AIO control block structure use in asynchronous I/O operations
 * we use it's in kernel mode instead of <aio.h> from standart library.
 * Now it use only for compatible socket interface.
 */
struct kiocb {
	struct list_head  ki_run_list;
	long              ki_flags;
	int               ki_users;
	unsigned          ki_key;   /* id of this request */
};

#endif /* LINUX_AIO_H_ */

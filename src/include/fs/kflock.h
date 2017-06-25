/**
 * @file
 * @brief
 *
 * @date Apr 3, 2014
 * @author Anton Bondarev
 */

#ifndef KFLOCK_H_
#define KFLOCK_H_


#include <fcntl.h>

#include <util/dlist.h>
#include <kernel/thread/sync/mutex.h>
#include <sched.h>
/*
typedef struct kflock_exclusive {
	struct mutex      lock;
	struct dlist_head kflock_link;
	short             whence;
	off_t             start;
	off_t             len;
	pid_t             pid;
} kflock_exclusive_t;

typedef struct kflock_shared {
	struct dlist_head kflock_link;
	short             whence;
	off_t             start;
	off_t             len;
	pid_t             pid;
} kflock_shared_t;

typedef union kflock_both {
	kflock_exclusive_t exlock;
	kflock_shared_t    shlock;
} kflock_both_t;

typedef struct kflock {
	struct dlist_head exlock_list;
	long              exlock_count;
	struct dlist_head shlock_list;
	long              shlock_count;
	spinlock_t        flock_guard;
} kflock_t;
*/

typedef struct kflock_lock {
	/*short             type;
	short             whence;
	off_t             start;
	off_t             len;
	pid_t             pid;*/
	struct flock      flock;
	struct dlist_head kflock_link;
	//struct wait_queue wq;
} kflock_lock_t;

typedef struct kflock {
	struct dlist_head locks;
	spinlock_t        kflock_guard;
} kflock_t;


/**
 * @brief Fcntl functions implementation
 *
 * @param fd file descriptor for operation
 * @param cmd command to describe action you would like to perform
 * @param ... one or more command specific options
 *
 * @return zero if it was no errors or according error code
 */
int vfs_fcntl(int fd, int cmd, ... /* arg */ );

#endif /* KFLOCK_H_ */

/**
 * @file
 * @brief
 *
 * @date   24.09.2013
 * @author Vita Loginova
 */

#ifndef MUTEXATTR_H_
#define MUTEXATTR_H_

/* Mutex does not detect deadlock. A thread attempting to relock this mutex
 * without first unlocking it will deadlock */
#define PTHREAD_MUTEX_NORMAL 		0x0000
#define PTHREAD_MUTEX_ERRORCHECK 	0x0010
/* Recursive locking acceptable. Multiple locks of this mutex require the same
 * number of unlocks */
#define PTHREAD_MUTEX_RECURSIVE 	0x0020
#define PTHREAD_MUTEX_DEFAULT 		0x0030


struct mutexattr {
	int type;
};

extern int mutexattr_init(struct mutexattr*);
extern int mutexattr_destroy(struct mutexattr*);
extern int mutexattr_gettype(const struct mutexattr*, int*);
extern int mutexattr_settype(struct mutexattr*, int);

#endif /* MUTEXATTR_H_ */

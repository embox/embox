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
#define MUTEX_NORMAL 		0x0000
/* Mutex provides error checking.  If a thread attempts to relock a mutex that
 * it has already locked, an error shall be returned. If a thread attempts to
 * unlock a mutex that it has not locked or a mutex which is unlocked, an error
 * shall be returned. */
#define MUTEX_ERRORCHECK 	0x0010
/* Recursive locking acceptable. Multiple locks of this mutex require the same
 * number of unlocks. If a thread attempts to unlock a mutex that it has not
 * locked or a mutex which is unlocked, an error shall be returned */
#define MUTEX_RECURSIVE 	0x0020
/* Doesn't provide any checking, actually it is the same as MUTEX_NORMAL */
#define MUTEX_DEFAULT 		0x0040


struct mutexattr {
	int type;
};

/**
 * initializes given mutexattr with the default values
 * @param mutexattr mutexattr to initialize
 */
extern int mutexattr_init(struct mutexattr*);
/**
 * copies given mutexattr with the default values
 * @param mutexattr mutexattr to initialize
 */
extern int mutexattr_copy(const struct mutexattr*, struct mutexattr*);
extern int mutexattr_destroy(struct mutexattr*);
extern int mutexattr_gettype(const struct mutexattr*, int*);
extern int mutexattr_settype(struct mutexattr*, int);

#endif /* MUTEXATTR_H_ */

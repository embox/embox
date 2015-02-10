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
#define MUTEX_NORMAL 		0
/* Mutex provides error checking.  If a thread attempts to relock a mutex that
 * it has already locked, an error shall be returned. If a thread attempts to
 * unlock a mutex that it has not locked or a mutex which is unlocked, an error
 * shall be returned. */
#define MUTEX_ERRORCHECK 	1
/* Recursive locking acceptable. Multiple locks of this mutex require the same
 * number of unlocks. If a thread attempts to unlock a mutex that it has not
 * locked or a mutex which is unlocked, an error shall be returned */
#define MUTEX_RECURSIVE 	2
/* Doesn't provide any checking, actually it is the same as MUTEX_NORMAL */
#define MUTEX_DEFAULT 		MUTEX_NORMAL


struct mutexattr {
	int type;
};

/**
 * initializes given mutexattr with the default values
 * @param mutexattr attr to initialize
 */
extern int mutexattr_init(struct mutexattr *attr);
/**
 * copies values of source mutexattr to destination
 * @param mutexattr source_attr source mutexattr
 * @param mutexattr dest_attr destination mutexattr
 */
extern int mutexattr_copy(const struct mutexattr *source_attr,
		struct mutexattr *dest_attr);
/**
 * makes given mutexattr uninitialized
 * @param mutexattr attr to destroy
 */
extern int mutexattr_destroy(struct mutexattr *attr);
/**
 * gets current mutexattr type by int type
 * @param mutexattr attr with a type to get
 * @param type will contain mutexattr type
 */
extern int mutexattr_gettype(const struct mutexattr *attr, int *type);
/**
 * sets given type to mutexattr type
 * @param mutexattr will contain new type
 * @param type type to set
 */
extern int mutexattr_settype(struct mutexattr *attr, int type);

#endif /* MUTEXATTR_H_ */

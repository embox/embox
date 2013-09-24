/**
 * @file
 * @brief
 *
 * @date   24.09.2013
 * @author Vita Loginova
 */

#ifndef MUTEXATTR_H_
#define MUTEXATTR_H_

#define PTHREAD_MUTEX_DEFAULT 0
#define PTHREAD_MUTEX_ERRORCHECK 1
#define PTHREAD_MUTEX_RECURSIVE 2
#define PTHREAD_MUTEX_NORMAL 3

struct mutexattr {
	int type;
};

extern int mutexattr_init(struct mutexattr*);
extern int mutexattr_destroy(struct mutexattr*);

#endif /* MUTEXATTR_H_ */

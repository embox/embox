/**
 * @file
 * @brief
 *
 * @date 2.12.2020
 * @author Suraaj K S
 */

#ifndef LIB_SYSCTL_H_
#define LIB_SYSCTL_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

struct sysct {
	const char *processor;
	const char *platform; 
	const char *system;
};


extern int sysctl(struct sysct *);

__END_DECLS

#endif /* LIB_SYSCTL_H_ */

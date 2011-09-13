/**
 * @file
 *
 * @brief
 *
 * @date 12.09.2011
 * @author Anton Bondarev
 */

#ifndef SYS_UTSNAME_H_
#define SYS_UTSNAME_H_

struct utsname {
	const char  *sysname;  /* Name of this implementation of the operating system. */
	const char  *nodename; /* Name of this node within the communications network to which this node is attached, if any. */
	const char  *release;  /* Current release level of this implementation. */
	const char  *version;  /* Current version level of this release. */
	const char  *machine;  /*  Name of the hardware type on which the system is running. */
};

extern int uname(struct utsname *);

#endif /* SYS_UTSNAME_H_ */

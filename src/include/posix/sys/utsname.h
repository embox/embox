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
	char  sysname[10];  /* Name of this implementation of the operating system. */
	char  nodename[10]; /* Name of this node within the communications network to which this node is attached, if any. */
	char  release[10];  /* Current release level of this implementation. */
	char  version[10];  /* Current version level of this release. */
	char  machine[10];  /*  Name of the hardware type on which the system is running. */
};

extern int uname(struct utsname *);

#endif /* SYS_UTSNAME_H_ */

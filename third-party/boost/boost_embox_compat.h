/*
 * samba_embox_compat.h
 *
 *  Created on: 21 mars 2013
 *      Author: fsulima
 */

#ifndef SAMBA_EMBOX_COMPAT_H_
#define SAMBA_EMBOX_COMPAT_H_

#ifdef linux
#undef linux
#endif

#ifdef __linux__
#undef __linux__
#endif

#if 1
#define DPRINT() printf(">>> samba CALL %s\n", __FUNCTION__)
#else
#define DPRINT()
#endif

#include <stdio.h>

//This can be used to derive sysconf from STLport
using std::sysconf;
/*
static inline
long sysconf(int name) {
//#define _SC_CLK_TCK 1
	printf(">>> sysconf(%d)\n", name);
	switch (name) {
		case _SC_PAGESIZE: return 4096;
//		case _SC_CLK_TCK: return
		default: break;
	}
	return -1;
}
*/


namespace std {
	static inline
	size_t strxfrm(char *dest, const char *src, size_t n);
}


#endif /* SAMBA_EMBOX_COMPAT_H_ */

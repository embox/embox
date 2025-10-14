#ifndef ARDUPILOT_EMBOX_COMPAT_H
#define ARDUPILOT_EMBOX_COMPAT_H

#include <sys/cdefs.h>

__BEGIN_DECLS
extern ssize_t readlink(const char *path, char *buf,
		       size_t bufsize);
__END_DECLS

#define CRTSCTS	  0x80000000	/* flow control */

#endif
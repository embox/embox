#ifndef ARDUPILOT_EMBOX_COMPAT_H
#define ARDUPILOT_EMBOX_COMPAT_H

extern ssize_t readlink(const char *path, char *buf,
		       size_t bufsize);

#define CRTSCTS	  0x80000000	/* flow control */

#endif
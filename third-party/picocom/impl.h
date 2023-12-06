static inline char *ttyname(int fd) {
	static char buf[16];
	(void) fd;
	return strcpy(buf, "/dev/ttyS0");
}

static int tcdrain (int __fd) {
	return 0;
}

static int tcsendbreak (int __fd, int __duration) {
	return 0;
}

/**
 * @file
 *
 * @date Dec 19, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_POSIX_INCLUDE_SYS_UN_H_
#define SRC_COMPAT_POSIX_INCLUDE_SYS_UN_H_

struct sockaddr_un {
	unsigned short sun_family;  /* AF_UNIX */
	/* The size of sun_path has intentionally been left undefined.
	 * This is because different implementations use different sizes.
	 * For example, 4.3 BSD uses a size of 108, and 4.4 BSD uses a size of 104.
	 * Since most implementations originate from BSD versions, the size is
	 * typically in the range 92 to 108. Applications should not assume
	 * a particular length for sun_path or assume that it can hold
	 * {_POSIX_PATH_MAX} characters (255).
	 */
	char sun_path[108];
};

#endif /* SRC_COMPAT_POSIX_INCLUDE_SYS_UN_H_ */

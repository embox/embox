/**
 * @file
 *
 * @date Oct 28, 2013
 * @author: Anton Bondarev
 */

#ifndef POSIX_LIBGEN_H_
#define POSIX_LIBGEN_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

extern char *basename(char *path);

extern char *dirname(char *path);

__END_DECLS

#endif /* POSIX_LIBGEN_H_ */

#ifndef DIRENT_IMPL_H_
#define DIRENT_IMPL_H_

#include <sys/types.h>
#include <sys/cdefs.h>

#include <fs/path.h>

struct DIR_struct {
	struct dirent current;
	struct path path;
};

#endif /* DIRENT_IMPL_H_ */

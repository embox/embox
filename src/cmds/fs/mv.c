/**
 * @file
 * @brief Move or rename file.
 *
 * @date 18.12.12
 * @author Ivan Tretyakov
 */


#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fs/perm.h>
#include <fs/vfs.h>
#include <fs/fsop.h>
#include <util/array.h>

#define MV_RECURSIVE	(0x1 << 0)
#define MV_FORCE		(0x1 << 1)

/* Iterate through array from position 'start', 'count' times
 * and set 'value' to current value on each step */

static void print_usage(void) {
	printf("Usage: mv [-rfh] path\n"
			"\t-r - recursive\n"
			"\t-f - do not prompt before overwrite\n"
			"\t-h - print this help\n");
}

int main(int argc, char **argv) {
	int opt, rc;
	unsigned int flags = 0;
	char *oldpathcopy = NULL, *newpathcopy = NULL;
	char *opc_free, *npc_free;
	char *oldpath, *newpath = argv[argc - 1];
	struct path oldnode, newnode;

	newpathcopy = strdup(newpath);
	npc_free = newpathcopy;

	rc = fs_perm_lookup((const char *) newpathcopy,
			(const char **) &newpathcopy, &newnode);
	free(npc_free);
	if (-ENOENT == rc) {
		newnode.node = NULL;
	} else if (0 != rc && -ENOENT != rc) {
		return rc;
	}

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "rfh"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return -ENOERR;
		case 'r':
			flags |= MV_RECURSIVE;
			break;
		case 'f':
			flags |= MV_FORCE;
			break;
		default:
			printf("mv: invalid option -- '%c'\n", optopt);
			return -EINVAL;
		}
	}

	/* If we are copying many files destination
	 * should exists and should be directory */
	if (argc - optind >= 3 &&
			(NULL == newnode.node || (! node_is_directory(newnode.node)))) {
		fprintf(stderr,	"mv: target '%s' is not a directory\n", newpath);
		return -EINVAL;
	}

	/* If there is directory in old paths than MV_RECURSIVE should be set */
	array_foreach(oldpath, (argv + optind), (argc - optind - 1)) {
		/* We wouldn't rename directory without -r */
		if (! (flags & MV_RECURSIVE)) {
			oldpathcopy = strdup(oldpath);
			opc_free = oldpathcopy;
			rc = fs_perm_lookup((const char *) oldpathcopy,
					(const char **) &oldpathcopy, &oldnode);
			free(opc_free);
			if (-ENOENT == rc) {
				fprintf(stderr,	"mv: source '%s' does not exist\n", oldpath);
				return rc;
			} else if (0 != rc) {
				return rc;
			}
			if (node_is_directory(oldnode.node)) {
				fprintf(stderr,	"mv: source '%s' is a directory\n", oldpath);
				return -EINVAL;
			}
		}
	}

	/* If new path exists and MV_FORCE is not set */
	if (NULL != newnode.node && (! node_is_directory(newnode.node))) {
		if (! (flags & MV_FORCE)) {
			fprintf(stderr,	"mv: destination path '%s' exists\n", newpath);
			return -EINVAL;
		}
		remove(newpath);
	}

	array_foreach(oldpath, (argv + optind), (argc - optind - 1)) {
		if (-1 == rename(oldpath, newpath)) {
			return -errno;
		}
	}

	return -ENOERR;
}

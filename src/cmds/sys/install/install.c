/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    03.06.2014
 */

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fs/mount.h>
#include <dirent.h>
#include <sys/stat.h>

#define INITFS_NAME "initfs"

#define MSG_PREFIX "install: "

static void cp_all_content_recursive(const char *src, const char *dst) {
	char cmd_buf[512];
	struct dirent *dent;
	DIR *dir;

	dir = opendir(src);
	if (!dir) {
		return;
	}

	while (NULL != (dent = readdir(dir))) {
		snprintf(cmd_buf, sizeof(cmd_buf), "cp %s/%s %s", src, dent->d_name, dst);
#ifdef DRY_RUN
		fprintf(stderr, MSG_PREFIX "going to call \"%s\"\n", cmd_buf);
#else
		system(cmd_buf);
#endif
	}
}

int main(int argc, char *argv[]) {
	int opt;
	const char *base_root = "/initfs";
	const char *target_root = "/";
	bool mount_manage = true;
	bool mountpoint_manage = true;

	while (-1 != (opt = getopt(argc, argv, "mb:t:"))) {
		switch(opt) {
		case 'm':
			mount_manage = false;
			break;
		case 'b':
			base_root = optarg;
			break;
		case 't':
			target_root = optarg;
			break;
		default:
			fprintf(stderr, MSG_PREFIX "unknown option\n");
		}
	}

	if (mount_manage) {
		int res;

		res = mkdir(base_root, 0755);
		if (res != 0) {
			if (res == -EEXIST) {
				mountpoint_manage = false;
			} else {
				fprintf(stderr, MSG_PREFIX "can't create mountpoint while mounting."
						"Try -m to disable automatic mounting\n");
				return -res;
			}
		}

		res = mount("none", (char *) base_root, INITFS_NAME);
		if (res != 0) {
			fprintf(stderr, MSG_PREFIX "can't mount."
					"Try -m to disable automatic mounting\n");
			return -res;
		}
	}

	cp_all_content_recursive(base_root, target_root);

	if (mount_manage) {

		umount((char *) base_root);

		if (mountpoint_manage) {
			rmdir(base_root);
		}
	}

	return 0;
}


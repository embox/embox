 /**
 * @file
 * @brief Mini cp implementation for Embox.
 *
 * @date 22.08.09
 * @author Roman Evstifeev
 * @author Anton Kozlov
 */

#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <libgen.h>
#include <dirent.h>

#define CP_BLOCK_SIZE 512
#define MSG_INFO "cp: "

#define CP_MKDIR_MODE 0777

static void print_usage(void) {
	printf("Usage:\n"
		"\tcp [-h] [-r|-R] SOURCE DEST\n"
		"\tcp [-h] [-r|-R] SOURCE DIR\n");
}

static char cp_g_src_path[PATH_MAX], cp_g_dst_path[PATH_MAX];

static char *path_append_store(char *path) {
	return path + strlen(path);
}

static void path_append_restore(char *storeval) {
	*storeval = '\0';
}

static int path_type(const char *path) {
	struct stat st;

	if (0 != stat(path, &st)) {
		return -errno;
	}

	return st.st_mode & S_IFMT;
}

static void cp_target_add_basename(const char *src, char *dst) {
	char bname_buf[PATH_MAX];
	char *bname;

	if (path_type(dst) == S_IFDIR) {
		strcpy(bname_buf, src);
		bname = basename(bname_buf);

		strcat(dst, "/");
		strcat(dst, bname);
	}
}

static int cp_file(const char *src_path, const char *dst_path) {
	char buf[CP_BLOCK_SIZE];
	int src_file, dst_file;
	int ret, bytesread;

	if (-1 == (src_file = open(src_path, O_RDONLY)))  {
		printf("can't open file %s\n",src_path);
		return -errno;
	}

	if (0 > (dst_file = creat(dst_path, 0))) {
		ret = -errno;
		printf("can't open file %s\n",dst_path);
		close(src_file);
		return ret;
	}

	lseek(dst_file, 0, SEEK_SET);

	/* buf optimized for whole block write */
	bytesread = 0;
	while ((bytesread = read(src_file, buf, sizeof(buf))) > 0) {
		if (write (dst_file, buf, bytesread)<=0) {
			printf ("WR ERR!\n");
			break;
		}
	}

	ret = 0;
	if (fsync(dst_file)) {
		ret = -errno;
	}

	close(src_file);
	close(dst_file);

	return ret;
}

static int cp_recursive(char *src, char *dst) {
	int src_type;

	src_type = path_type(src);
	if (src_type == S_IFDIR) {
		char *src_store, *dst_store;
		struct dirent *dent;
		DIR *dir;

		dir = opendir(src);
		if (!dir) {
			return -errno;
		}

		src_store = path_append_store(src);
		dst_store = path_append_store(dst);

		while (NULL != (dent = readdir(dir))) {

			strcpy(src_store, "/");
			strcat(src_store, dent->d_name);

			strcpy(dst_store, "/");
			strcat(dst_store, dent->d_name);

			switch (path_type(src)) {
			case S_IFDIR:
				if (0 > mkdir(dst, CP_MKDIR_MODE)) {
					return -errno;
				}
				cp_recursive(src, dst);
				break;
			case S_IFREG:
				cp_file(src, dst);
				break;
			default:
				break;
			}
		}
		closedir(dir);

		path_append_restore(src_store);
		path_append_restore(dst_store);

	} else if (src_type == S_IFREG) {
		return cp_file(src, dst);
	} else if (src_type > 0) {
		fprintf(stderr, MSG_INFO "only directiry or file can be copied\n");
		return 0;
	} else {
		return src_type;
	}

	return 0;
}

int main(int argc, char **argv) {
	int opt;
	bool recursively = false;

	while (-1 != (opt = getopt(argc, argv, "hrR"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		case 'R':
		case 'r':
			recursively = true;
			break;
		default:
			break;
		}
	}

	if (argc < 3) {
		print_usage();
		return -EINVAL;
	}

	strcpy(cp_g_src_path, argv[argc - 2]);
	strcpy(cp_g_dst_path, argv[argc - 1]);

	cp_target_add_basename(cp_g_src_path, cp_g_dst_path);

	if (recursively) {
		int src_type, dst_type;

		src_type = path_type(cp_g_src_path);
		dst_type = path_type(cp_g_dst_path);

		if (src_type == S_IFDIR && dst_type < 0) {
			if (0 > mkdir(cp_g_dst_path, CP_MKDIR_MODE)) {
				return -errno;
			}
		}

		return cp_recursive(cp_g_src_path, cp_g_dst_path);
	} else {
		return cp_file(cp_g_src_path, cp_g_dst_path);
	}
}

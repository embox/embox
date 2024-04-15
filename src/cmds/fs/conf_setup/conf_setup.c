/**
 * @file
 *
 * @date Jul 29, 2021
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include <lib/libds/array.h>

#include <conf_setup.inc>

#define CP_BLOCK_SIZE  32

static int cp_file(const char *src_path, const char *dst_path) {
	char buf[CP_BLOCK_SIZE];
	int src_file, dst_file;
	int ret, bytesread;

	if (-1 == (src_file = open(src_path, O_RDONLY)))  {
		printf("can't open file %s\n",src_path);
		return -1;
	}

	if (0 > (dst_file = creat(dst_path, 0))) {
		ret = -1;
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


int main(int argc, char **argv) {
	int i;
	struct stat st;
	char conf_file_name[PATH_MAX];

	for (i = 0; i < ARRAY_SIZE(conf_files); i++) {
		conf_file_name[0] = '\0';
		strcat(conf_file_name, CONF_FOLDER);
		strcat(conf_file_name, conf_files[i]);
		if (stat(conf_file_name, &st)) {
			cp_file(conf_files[i], conf_file_name);
		}
	}

	return 0;
}

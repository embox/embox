/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.07.2014
 */
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static int chown_do(char *files[], uid_t owner_id, gid_t group_id, bool is_group_set) {
	char **file_p;
	int res;

	for (file_p = files; *file_p != NULL; file_p++) {
		char *file = *file_p;
		struct stat st_buf;

		if(!is_group_set) {
			res = stat(file, &st_buf);
			if (res) {
				fprintf(stderr, "stat(%s, ..) return error = %d\n",
						file, errno);
				return errno;
			}
			group_id = st_buf.st_gid;
		}

		res = chown(file, owner_id, group_id);
		if (res) {
			fprintf(stderr, "chown(%s, %d, %d) return error = %d\n",
					file, owner_id, group_id, errno);
			return errno;
		}
	}

	return 0;
}

int main(int argc, char *argv[]) {
	uid_t owner_id;
	bool is_group_set = false;
	gid_t group_id = 0; /* TODO use current user group id */
	char *delim;

	if (argc < 3) {
		return -EINVAL;
	}

	owner_id = strtol(argv[1], &delim, 0);
	if (delim == argv[1]) {
		fprintf(stderr, "first argument should be owner id\n");
		return -EINVAL;
	}

	if (*delim == ':') {
		char *end;
		delim += 1;

		group_id = strtol(delim, &end, 0);
		if (delim == end) {
			fprintf(stderr, "after semicolon should be group id\n");
			return -EINVAL;
		}

		is_group_set = true;
	}

	return chown_do(argv + 2, owner_id, group_id, is_group_set);
}

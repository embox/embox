/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.07.2014
 */

#include <unistd.h>
#include <stdlib.h>
#include <fs/path.h>
#include <fs/vfs.h>

static int chown_do(char *files[], uid_t owner_id, gid_t group_id, bool is_group_set) {
	char **file_p;

	for (file_p = files; *file_p != NULL; file_p++) {
		char *file = *file_p;
		struct path node_path;
		int ret;

		ret = vfs_lookup(file, &node_path);
		if (ret) {
			fprintf(stderr, "Can't open %s: %s\n", file, strerror(-ret));
			return ret;
		}

		node_path.node->uid = owner_id;
		if (is_group_set) {
			node_path.node->gid = group_id;
		}
	}

	return 0;
}

int main(int argc, char *argv[]) {
	uid_t owner_id;
	bool is_group_set;
	gid_t group_id;
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

/**
 * @file
 * @brief
 *
 * @date 25.02.14
 * @author Vita Loginova
 */

#include <fs/perm.h>
#include <fs/hlpr_path.h>
#include <fs/vfs.h>
#include <fs/node.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <libgen.h>
#include <pwd.h>

struct mode {
	mode_t type;
	mode_t mask;
	mode_t dir_mask;
	void (*func)(mode_t, mode_t, node_t*);
};

static void add(mode_t mode, mode_t type, node_t *node) {
	node->mode |= mode & type;
}

static void sub(mode_t mode, mode_t type, node_t *node) {
	node->mode &= ~(mode & type);
}

static void assign(mode_t mode, mode_t type, node_t *node) {
	node->mode &= ~type;
	node->mode |= mode & type;
}

static void help_invalid_mode(const char *mode) {
	printf("chmod: invalid mode: '%s'\n"
			"Try 'chmod -h' for more information.\n", mode);
}

static void help_cannot_access(const char *path, const char *error) {
	printf("chmod: cannot access '%s': %s\n", path, error);
}

static void help(void) {
	printf("Usage: chmod [OPTION]... MODE[,MODE]... FILE...\n");
}

static int parse_mode(char *mode, struct mode *modes, int size) {
	char *tmp_mode = mode;
	int count = 0;

parse:
	if (count >= size) {
		printf("chmod: too many modes: %s\n", mode);
		return -1;
	}

	modes[count].type = 0;
	modes[count].mask = 0;
	modes[count].dir_mask = 0;
	modes[count].func = NULL;

	while (*tmp_mode != '=' && *tmp_mode != '+' && *tmp_mode != '-') {
		switch(*(tmp_mode++)) {
		case 'u':
			modes[count].type |= 0700;
			break;
		case 'g':
			modes[count].type |= 0070;
			break;
		case 'o':
			modes[count].type |= 0007;
			break;
		case 'a':
			modes[count].type |= 0777;
			break;
		default:
			help_invalid_mode(mode);
			return -1;
		}
	}

	if (!modes[count].type) {
		modes[count].type |= 0777;
	}

	switch(*(tmp_mode++)) {
	case '+':
		modes[count].func = add;
		break;
	case '-':
		modes[count].func = sub;
		break;
	case '=':
		modes[count].func = assign;
		break;
	default:
		help_invalid_mode(mode);
		return -1;
	}

	while (*tmp_mode != '\0') {
		switch(*(tmp_mode++)) {
		case 'r':
			modes[count].mask |= 0444;
			break;
		case 'w':
			modes[count].mask |= 0222;
			break;
		case 'x':
			modes[count].mask |= 0111;
			break;
		case 'X':
			modes[count].dir_mask |= 0111;
			break;
		case ',':
			count++;
			goto parse;
		default:
			help_invalid_mode(mode);
			return -1;
		}
	}

	return count + 1;
}

static int find(struct path *root, const char *path, struct path *nodelast) {
	struct path *node_path = root;
	size_t len = 0;

	while (1) {
		path = path_next(path + len, &len);

		*nodelast = *node_path;

		if (!path) {
			break;
		}

		vfs_lookup_childn(node_path, path, len, node_path);
		if (NULL == node_path->node) {
			return -EACCES;
		}
	}

	return 0;
}

static int change_mode(struct path *node_path, int is_recursive,
		struct mode *modes, int count);

static int change_mode_recursive(struct path *node_path, int is_recursive,
		struct mode *modes, int count) {
	struct path child;
	struct tree_link *link;

	child = *node_path;
	if_mounted_follow_down(&child);

	dlist_foreach_entry(link, &child.node->tree_link.children, list_link) {
		child.node = tree_element(link, node_t, tree_link);
		change_mode(&child, is_recursive, modes, count);
	}

	return 0;
}

static int is_permitted(node_t *node) {
	uid_t uid = geteuid();
	struct passwd pwd, *result;
	char buf[80];

	if (0 != getpwuid_r(uid, &pwd, buf, 80, &result)) {
		return 0;
	}

	return uid == node->uid || pwd.pw_gid == 0;
}

static void apply_modes(node_t *node, struct mode *modes, int count, int is_dir) {
	if (is_dir) {
		for (int i = 0; i < count; i++) {
			modes[i].func(modes[i].mask | modes[i].dir_mask, modes[i].type, node);
		}
	} else {
		for (int i = 0; i < count; i++) {
			modes[i].func(modes[i].mask, modes[i].type, node);
		}
	}
}

static int change_mode(struct path *node_path, int is_recursive,
		struct mode *modes, int count) {
	char path[PATH_MAX];

	if (!is_permitted(node_path->node)) {
		vfs_get_path_by_node(node_path, path);
		help_cannot_access(path, "Permission denied");
		return 0;
	}

	if (node_is_directory(node_path->node)) {
		if (is_recursive) {
			change_mode_recursive(node_path, is_recursive, modes, count);
		}

		apply_modes(node_path->node, modes, count, 1);
		return 0;
	}

	apply_modes(node_path->node, modes, count, 0);

	return 0;
}

int main(int argc, char **argv) {
	struct path node_path, leaf;
	int opt, count, is_recursive = 0, modes_size = 10;
	struct mode modes[modes_size];
	char *path, *mode;

	if (argc <= 1) {
		help();
		return 0;
	}

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "Rh"))) {
		switch (opt) {
		case 'R':
			is_recursive = 1;
			break;
		case 'h':
			help();
			return 0;
		default:
			printf("chmod: invalid option -%c\n", optopt);
			help();
			return 0;
		}
	}

	if (optind + 2 != argc) {
		help();
		return 0;
	}

	mode = argv[optind];
	path = argv[optind + 1];

	vfs_get_leaf_path(&leaf);

	if (0 > find(&leaf, path, &node_path)) {
		help_cannot_access(path, "No such file or directory");
		return 0;
	}

	if (0 > (count = parse_mode(mode, modes, modes_size))) {
		return 0;
	}

	change_mode(&node_path, is_recursive, modes, count);

	return 0;
}

/**
 * @file
 * @brief
 *
 * @date 25.02.14
 * @author Vita Loginova
 */

#include <embox/cmd.h>

#include <fs/perm.h>
#include <fs/path.h>
#include <fs/vfs.h>
#include <fs/node.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <libgen.h>
#include <pwd.h>

EMBOX_CMD(exec);

static void add(unsigned int mode, unsigned int type, node_t *node) {
	node->mode |= mode & type;
}

static void sub(unsigned int mode, unsigned int type, node_t *node) {
	node->mode &= ~(mode & type);
}

static void assign(unsigned int mode, unsigned int type, node_t *node) {
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

static int __change_mode(char *mode, node_t *node) {
	unsigned int mask_type, mask_mode;
	char *tmp_mode = mode;
	void (*func)(unsigned int, unsigned int, node_t*);

parse:
	mask_type = 0;
	mask_mode = 0;

	while (*tmp_mode != '=' && *tmp_mode != '+' && *tmp_mode != '-') {
		switch(*(tmp_mode++)) {
		case 'u':
			mask_type |= 0700;
			break;
		case 'g':
			mask_type |= 0070;
			break;
		case 'o':
			mask_type |= 0007;
			break;
		case 'a':
			mask_type |= 0777;
			break;
		default:
			help_invalid_mode(mode);
			return 0;
		}
	}

	if (!mask_type) {
		mask_type |= 0777;
	}

	switch(*(tmp_mode++)) {
	case '+':
		func = add;
		break;
	case '-':
		func = sub;
		break;
	case '=':
		func = assign;
		break;
	default:
		help_invalid_mode(mode);
		return 0;
	}

	while (*tmp_mode != '\0') {
		switch(*(tmp_mode++)) {
		case 'r':
			mask_mode |= 0444;
			break;
		case 'w':
			mask_mode |= 0222;
			break;
		case 'x':
			mask_mode |= 0111;
			break;
		case 'X':
			if (node_is_directory(node)) {
				mask_mode |= 0111;
			}
			break;
		case ',':
			func(mask_mode, mask_type, node);
			goto parse;
			break;
		default:
			help_invalid_mode(mode);
			return 0;
		}
	}

	func(mask_mode, mask_type, node);

	return 0;
}

static int find(struct node *root, const char *path, struct node **nodelast) {
	struct node *node;
	size_t len = 0;

	while (1) {
		path = path_next(path + len, &len);

		*nodelast = node;

		if (!path) {
			break;
		}

		if (NULL == (node = vfs_lookup_childn(node, path, len))) {
			return -EACCES;
		}
	}

	return 0;
}

static int change_mode(node_t *node, char *mode, int is_recursive);

static int change_mode_recurse(struct tree_link *node, char *mode, int is_recursive) {
	struct tree_link *link;

	assert(node);

	list_foreach(link, &node->children, list_link) {
		change_mode(tree_element(link, node_t, tree_link), mode, is_recursive);
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

static int change_mode(node_t *node, char *mode, int is_recursive) {
	char path[PATH_MAX];

	if (!is_permitted(node)) {
		vfs_get_path_by_node(node, path);
		help_cannot_access(path, "Permission denied");
		return 0;
	}

	if (node_is_directory(node)) {
		if (is_recursive) {
			change_mode_recurse(&(node->tree_link), mode, is_recursive);
		}

		__change_mode(mode, node);
		return 0;
	}

	if (node_is_file(node)) {
		__change_mode(mode, node);
		return 0;
	}

	vfs_get_path_by_node(node, path);
	help_cannot_access(path, "Is not file or directory");

	return 0;
}

static int exec(int argc, char **argv) {
	node_t *node;
	int opt, is_recursive = 0;
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

	if (0 > find(vfs_get_leaf(), path, &node)) {
		help_cannot_access(path, "No such file or directory");
		return 0;
	}

	change_mode(node, mode, is_recursive);

	return 0;
}

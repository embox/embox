/**
 * @file
 * @brief
 *
 * @date 25.02.14
 * @author Vita Loginova
 */

#include <embox/cmd.h>

#include <fs/perm.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fs/path.h>
#include <fs/vfs.h>

EMBOX_CMD(exec);

static void add(unsigned int mode, unsigned int type, node_t *node) {
	node->mode |= mode & type;
}

static void sub(unsigned int mode, unsigned int type, node_t *node) {
	node->mode &= ~(mode & type);
}

static void assign(unsigned int mode, unsigned int type, node_t *node) {
	node->mode &= ~type;
	node->mode |= type & mode;
}

static void help_invalid_mode(char *mode) {
	printf ("chmod: invalid mode: '%s'\n"
			"Try 'chmod --help' for more information.\n", mode);
}

static void help_cannot_access(char *path, char *error) {
	printf ("chmod: cannot access '%s': %s\n", path, error);
}

static int _chmod(char *mode, node_t *node) {
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

static int exec(int argc, char **argv) {
	node_t *node;
	int res;
	char* path = argv[2];
	char* mode = argv[1];

	if (0 != (res = fs_perm_lookup(vfs_get_leaf(), path, NULL, &node))) {
		switch(-res) {
		case EACCES:
			help_cannot_access(path, "Permission denied");
			break;
		case ENOENT:
			help_cannot_access(path, "No such file or directory");
			break;
		default:
			help_cannot_access(path, "Unexpected error");
			break;
		}
		return 0;
	}

	_chmod(mode, node);

	return 0;
}

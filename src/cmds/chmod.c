/*
 * chmod.c
 *
 *  Created on: Feb 25, 2014
 *      Author: vita
 */

#include <embox/cmd.h>

#include <fs/perm.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fs/path.h>
#include <fs/vfs.h>

EMBOX_CMD(exec);

static int add(unsigned int mode, unsigned int type, node_t *node) {
	node->mode |= mode & type;
	return 0;
}

static int sub(unsigned int mode, unsigned int type, node_t *node) {
	node->mode &= ~(mode & type);
	return 0;
}

static int assign(unsigned int mode, unsigned int type, node_t *node) {
	node->mode &= ~type;
	node->mode |= type & mode;
	return 0;
}

static int _chmod(char *mode, node_t *node) {
	unsigned int type = 0, _mode = 0;
	int (*func)(unsigned int, unsigned int, node_t*);

	while (mode[0] != '=' && mode[0] != '+' && mode[0] != '-') {
		switch(mode[0]) {
		case 'u':
			type |= 0700;
			break;
		case 'g':
			type |= 0070;
			break;
		case 'o':
			type |= 0007;
			break;
		case 'a':
			type |= 0777;
			break;
		default:
			printf("Parse error\n");
			return 0;
		}
		mode++;
	}

	if (!type) {
		type |= 0777;
	}

	switch(mode[0]) {
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
		printf("Parse error\n");
		return 0;
	}
	mode++;

	while (mode[0] != ',' && mode[0] != '\0') {
		switch(mode[0]) {
		case 'r':
			_mode |= 0444;
			break;
		case 'w':
			_mode |= 0222;
			break;
		case 'x':
			_mode |= 0111;
			break;
		default:
			printf("Parse error\n");
			return 0;
		}
		mode++;
	}

	func(_mode, type, node);

	return 0;
}

static int exec(int argc, char **argv) {
	node_t *node;
	int res;

	if (0 != (res = fs_perm_lookup(vfs_get_leaf(), argv[2], NULL, &node))) {
		printf("Lookup fail\n");
		return 0;
	}

	_chmod(argv[1], node);

	return 0;
}

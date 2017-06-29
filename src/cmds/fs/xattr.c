/**
 * @file
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 */

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <fs/fs_driver.h>
#include <fs/ext2.h>
#include <fs/xattr.h>
#include <sys/xattr.h>

#include <mem/page.h>
#include <mem/phymem.h>

#define XATTR_MAX_BSIZE  (1024*4)

#define XATTR_CMD_OP_HEX    (1 << 0)
#define XATTR_CMD_OP_VDISP  (1 << 1)

static unsigned int xattr_cmd_op_flags;

enum xattr_cmd_op {
	XATTR_CMD_UNSPECIFIED,
	XATTR_CMD_LIST,
	XATTR_CMD_CLEAR,
	XATTR_CMD_PRINT,
	XATTR_CMD_DELETE,
	XATTR_CMD_WRITE,
	XATTR_CMD_INVALID,
};

static void print_usage(void) {
	printf("Usage: xattr [-cdpwh] [-lx] [attr_name] [attr_value] file "
			"[file ...]\n");
}

static int xattr_print_value(const char *path, const char *name) {
	char rvalue[XATTR_MAX_VSIZE];
	size_t size;
	int rc;

	if (0 > (rc = getxattr(path, name, rvalue, XATTR_MAX_VSIZE))) {
		return -errno;
	}
	size = rc;

	rvalue[size] = '\0';

	if (XATTR_CMD_OP_HEX & xattr_cmd_op_flags) {
		char *ptr = rvalue;
		while(size--) {
			printf("%02hhX", (unsigned char) *ptr++);
			printf("|");
		}
	} else {
		printf("%s", rvalue);
	}

	return 0;
}

static int scan_hex_value(char *rvalue, const char *hex_str) {
	size_t size;
	unsigned char *dst;
	char *src, *end;

	if (0 == (size = strlen(hex_str))) {
		return 0;
	}

	src = (char *) hex_str;
	dst = (unsigned char *) rvalue;
	end = src + size;

	while(src < end) {
		sscanf(src, "%hhX", dst);
		dst++;
		src += 2;
	}

	return size % 2 ? size / 2 + 1 : size / 2;
}

static int xattr_do_delete(const char *path, const char *name) {
	if (-1 == setxattr(path, name, NULL, 0, XATTR_REMOVE)) {
		return -errno;
	}

	return 0;
}

static int xattr_do_write(const char *path, const char *name, const char *strvalue) {
	char rvalue[XATTR_MAX_VSIZE];
	size_t size;
	char *value;

	if (xattr_cmd_op_flags & XATTR_CMD_OP_HEX) {
		size = scan_hex_value(rvalue, strvalue);
		value = rvalue;
	} else {
		value = (char *) strvalue;
		size = strlen(value);
	}

	if (-1 == setxattr(path, name, value, size, 0) ) {
		return -errno;
	}

	return 0;
}

static int xattr_do_print(const char *path, const char *name) {
	int rc;
	rc = xattr_print_value(path, name);
	if (!rc) {
		printf("\n");
	}

	return rc;
}

static int xattr_do_iter(const char *path, int (*xattr_iter_fn)(const char *path, const char *name)) {
	const unsigned int page_n = XATTR_MAX_BSIZE / PAGE_SIZE() + 1;
	size_t len;
	char *list, *point;
	int rc;

	if (NULL == (list = phymem_alloc(page_n))) {
		return -ENOMEM;
	}
	memset(list, 0, PAGE_SIZE() * page_n);

	if (0 > listxattr(path, list, XATTR_MAX_BSIZE)) {
		rc = -errno;
		goto free_list;
	}

	point = list;
	while ((0 != (len = strlen(point))) && (point < list + XATTR_MAX_BSIZE)) {
		if (0 > (rc = xattr_iter_fn(path, point))) {
			goto free_list;
		}
		point += len + 1;
	}

	rc = 0;
free_list:
	phymem_free(list, page_n);
	return rc;
}

static int print_xattr_fn(const char *path, const char *name) {

	if (xattr_cmd_op_flags & XATTR_CMD_OP_VDISP) {
		int rc;

		printf("%s: ", name);
		rc = xattr_print_value(path, name);
		printf("\n");

		return rc;
	}

	printf("%s\n", name);

	return 0;
}

static int clear_xattr_fn(const char *path, const char *name) {
	int rc;

	if (0 > setxattr(path, name, 0, 0, XATTR_REMOVE)) {
		rc = -errno;
	} else {
		rc = 0;
	}

	return rc;
}


/*
 * xattr [-lx] file ... # list
 * xattr -c file ... # clear all xattr
 * xattr -p [-lx] attr_name file ... # print
 * xattr -d attr_name file ... # delete
 * xattr -w [-x] attr_name attr_value file ...  # write
 * xattr -h | -help
 */

static inline enum xattr_cmd_op xattr_check_op(enum xattr_cmd_op orig_op, enum xattr_cmd_op new_op) {
	if (orig_op == XATTR_CMD_UNSPECIFIED || orig_op == new_op) {
		return new_op;
	}

	return XATTR_CMD_INVALID;
}

int main(int argc, char **argv) {
	enum xattr_cmd_op cmd_op = XATTR_CMD_UNSPECIFIED;
	char **unpos_args;
	int opt;

	xattr_cmd_op_flags = 0;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "lxcdhpw"))) {
		switch (opt) {
		case 'c':
			cmd_op = xattr_check_op(cmd_op, XATTR_CMD_CLEAR);
			break;
		case 'p':
			cmd_op = xattr_check_op(cmd_op, XATTR_CMD_PRINT);
			break;
		case 'd':
			cmd_op = xattr_check_op(cmd_op, XATTR_CMD_DELETE);
			break;
		case 'w':
			cmd_op = xattr_check_op(cmd_op, XATTR_CMD_WRITE);
			break;
		case 'l':
			xattr_cmd_op_flags |= XATTR_CMD_OP_VDISP;
			break;
		case 'x':
			xattr_cmd_op_flags |= XATTR_CMD_OP_HEX;
			break;
		default:
		case 'h':
		case '?':
			print_usage();
			return 0;
		}
	}

	unpos_args = argv + optind;

	switch(cmd_op) {
	case XATTR_CMD_UNSPECIFIED:
	case XATTR_CMD_LIST:
		return xattr_do_iter(unpos_args[0], print_xattr_fn);
	case XATTR_CMD_CLEAR:
		return xattr_do_iter(unpos_args[0], clear_xattr_fn);
	case XATTR_CMD_PRINT:
		return xattr_do_print(unpos_args[1], unpos_args[0]);
	case XATTR_CMD_DELETE:
		return xattr_do_delete(unpos_args[1], unpos_args[0]);
	case XATTR_CMD_WRITE:
		return xattr_do_write(unpos_args[2], unpos_args[0], unpos_args[1]);
	case XATTR_CMD_INVALID:
		fprintf(stderr, "xattr: multiple operations specified\n");
	}
	return -1;
}

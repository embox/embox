/**
 * @file
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 */

#include <types.h>
#include <errno.h>
#include <unistd.h>
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
#include <cmd/mkfs.h>

#include <embox/cmd.h>

EMBOX_CMD(exec);


#define XATTR_CMD_LIST         0
#define XATTR_CMD_CLEAR  (1 << 0)
#define XATTR_CMD_PRINT  (1 << 1)
#define XATTR_CMD_DELETE (1 << 2)
#define XATTR_CMD_WRITE  (1 << 3)
#define XATTR_CMD_HEX    (1 << 4)
#define XATTR_CMD_VDISP  (1 << 5)

#define XATTR_CMD_MSK        0x0F

#define XATTR_MAX_BSIZE  (1024*4)

static void print_usage(void) {
	printf("Usage: xattr [-cdpwh] [-lx] [attr_name] [attr_value] file [file ...]\n");
}

static int check_invalid(int min_argc, int argc, char **argv) {
	if (min_argc > argc){
		printf("Invalid option `-%c' `%s'\n", optind, argv[optind]);
		print_usage();
		return EINVAL;
	} else {
		return 0;
	}
}

static void print_value(char *rvalue, size_t size, int op_flag) {
	if (0 == size) {
		return;
	}
	if (XATTR_CMD_HEX & op_flag) {
		while(size--) {
			printf("%02hhX", (unsigned char) *rvalue++);
			printf("|");
		}
	} else {
		printf("%s", rvalue);
	}
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

static int print_list(const char *path, char *point, char *rvalue, int op_flags) {
	int rc;
	size_t size;

	printf("%s", point);
	if (op_flags & XATTR_CMD_VDISP) {
		memset(rvalue, 0, XATTR_MAX_VSIZE);
		if (0 > (rc = getxattr(path, (const char *) point,
				rvalue, XATTR_MAX_VSIZE))) {
			return -errno;
		}
		size = rc;
		printf(": ");
		print_value(rvalue, size, op_flags);
	}
	printf("\n");

	return 0;
}

static int xattr_do_operation(const char *path, const char *name,
								const char *strvalue, int op_flags) {
	int flags;
	char *list;
	char rvalue[XATTR_MAX_VSIZE];
	char *value;
	char *point;
	int rc;
	size_t len, size;

	flags = 0;
	value = 0;

	if ((op_flags & XATTR_CMD_DELETE) || (op_flags &  XATTR_CMD_WRITE)) {
		if (op_flags & XATTR_CMD_DELETE) {
			flags = XATTR_REMOVE;
		} else {
			flags = XATTR_CREATE;
			if (op_flags & XATTR_CMD_HEX) {
				size = scan_hex_value(rvalue, strvalue);
				value = rvalue;
			} else {
				value = (char *) strvalue;
				size = strlen(value);
			}
		}

		if (-1 == setxattr(path, name, (const char *) value, size, flags) ) {
			return -errno;
		}
	} else if (op_flags & XATTR_CMD_PRINT) {
		memset(rvalue, 0, XATTR_MAX_VSIZE);
		if (0 > (rc = getxattr(path, name, rvalue, XATTR_MAX_VSIZE))) {
			return -errno;
		}
		size = rc;
		print_value(rvalue, size, op_flags);
		printf("\n");
		return 0;
	} else if (op_flags & XATTR_CMD_CLEAR || (0 == (op_flags & XATTR_CMD_MSK))) {
		if (NULL == (list = page_alloc_zero(__phymem_allocator,
							XATTR_MAX_BSIZE / PAGE_SIZE() + 1))) {
			return -ENOMEM;
		}
		if (0 > (rc = listxattr(path, list, XATTR_MAX_BSIZE))) {
			goto free_list;
		}

		point = list;
		while ((0 != (len = strlen(point))) &&
				(list + XATTR_MAX_BSIZE > point)) {
			if (op_flags & XATTR_CMD_CLEAR){ /* delete */
				flags = XATTR_REMOVE;
				if (0 > (setxattr(path, (const char *)point,
											0, 0, flags))) {
					rc = -errno;
					goto free_list;
				}
			} else { /* print list*/
				if (0 > (rc = print_list(path, point, rvalue, op_flags))) {
					goto free_list;
				}
			}
			point += len;
			point++;
		}
		goto free_list;
	}

	return -1;
	free_list:page_free(__phymem_allocator, list,
			XATTR_MAX_BSIZE / PAGE_SIZE() + 1);
	if (0 != rc) {
		return rc;
	}
	return 0;
}
/*
 * xattr [-lx] file ... # list
 * xattr -c file ... # clear all xattr
 * xattr -p [-lx] attr_name file ... # print
 * xattr -d attr_name file ... # delete
 * xattr -w [-x] attr_name attr_value file ...  # write
 * xattr -h | -help
 */

static int exec(int argc, char **argv) {
	int opt;
	int min_argc;
	int name_arg;
	unsigned int operation_flag;
	const char *path;
	const char *name;
	const char *value;

	min_argc = 2; /* path only for list */
	operation_flag = 0;
	path = name = value = NULL;
	name_arg = 1;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "lx:cdhpw:"))) {
		switch (opt) {
		case 'c':
			min_argc++;
			operation_flag |= XATTR_CMD_CLEAR;
			break;
		case 'p':
			min_argc += 2;
			name_arg++;
			operation_flag |=  XATTR_CMD_PRINT;
			break;
		case 'd':
			min_argc += 2;
			name_arg++;
			operation_flag |=  XATTR_CMD_DELETE;
			break;
		case 'w':
			min_argc += 3;
			name_arg++;
			operation_flag |=  XATTR_CMD_WRITE;
			break;
		case 'l':
			min_argc++;
			name_arg++;
			operation_flag |=  XATTR_CMD_VDISP;
			break;
		case 'x':
			min_argc++;
			name_arg++;
			operation_flag |=  XATTR_CMD_HEX;
			break;
		case '?':
			print_usage();
			return 0;
		case 'h':
			print_usage();
			return 0;
		default:
			return 0;
		}
	}

	if (check_invalid(min_argc, argc, argv)){
		return -EINVAL;
	}
	if (argc > 1) {
		if (operation_flag &  XATTR_CMD_WRITE) {
			value = argv[argc - 2];
		}
		name = argv[name_arg];

		/* last arg should be filename */
		path = argv[argc - 1];
	}

	return xattr_do_operation(path, name, value, operation_flag);
}


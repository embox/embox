/**
 * @file
 * @brief Read block devices block contents.
 *
 * @date 30.01.13
 * @author Andrey Gazukin
 * @author Anton Kozlov
 * 	-- output to file
 */

#include <errno.h>
#include <unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <stddef.h>

#include <lib/libds/array.h>

#define DD_DEFAULT_BS        512

#define DD_FORMAT_RAW        "raw"
#define DD_FORMAT_HEX_C      "hex_c"

struct dd_param {
	size_t bs;
	size_t count;
	size_t skip;
	size_t seek;

	const char *ifile;
	const char *ofile;

	const char *format;
};

struct dd_param_ent;
typedef void (*dd_param_t)(const struct dd_param_ent *dpent,
		struct dd_param *dp, const char *raw_val);

struct dd_param_ent {
	const char *name;
	off_t offset;
	dd_param_t type;
};

static int write_stdout(char *buff, size_t size, unsigned int addr) {
	size_t cnt;
	int i_substr, i_str;
	char *point, *substr_p;

	substr_p = point = buff;

	for (cnt = 0; cnt < size; cnt += 16) {
		printf("%08x: ", addr);
		for (i_str = 0; i_str < 2; i_str++) {
			for (i_substr = 0; i_substr < 8; i_substr++) {
				printf("%02hhx ", (unsigned char) *point++);
			}
			printf(" ");
		}

		printf("|");
		for (i_str = 0; i_str < 16; i_str++) {
			if ((((unsigned char) *substr_p) >= 32 &&
					((unsigned char) *substr_p) < 127)) { /*is not service simbol */
				printf("%c", *substr_p);
			} else {
				printf(".");
			}
			substr_p++;
		}
		printf("|\n");
		addr += 16;
	}

	return cnt;
}

#define DP_FIELD(dp, off, type) \
	((type) ((void *) dp + off))

static void dd_param_type_int(const struct dd_param_ent *dpent,
		struct dd_param *dp, const char *raw_val) {
	*DP_FIELD(dp, dpent->offset, size_t *) = strtol(raw_val, NULL, 0);
}

static void dd_param_type_str(const struct dd_param_ent *dpent,
		struct dd_param *dp, const char *raw_val) {
	*DP_FIELD(dp, dpent->offset, const char **) = raw_val;
}

#define DD_PARAM(name, type) \
	_DD_PARAM(# name, name, type)

#define _DD_PARAM(str_name, field_name, type) \
	{ str_name, offsetof(struct dd_param, field_name), type }

static const struct dd_param_ent dd_param_list[] = {
	_DD_PARAM("if", ifile, dd_param_type_str),
	_DD_PARAM("of", ofile, dd_param_type_str),
	DD_PARAM(bs, dd_param_type_int),
	DD_PARAM(count, dd_param_type_int),
	DD_PARAM(skip, dd_param_type_int),
	DD_PARAM(seek, dd_param_type_int),
	DD_PARAM(format, dd_param_type_str)
};

static const struct dd_param_ent *dd_param_ent_find(const char *name) {
	const struct dd_param_ent *dpent;
	for (dpent = dd_param_list;
			dpent < dd_param_list + ARRAY_SIZE(dd_param_list);
			dpent++) {
		if (!strcmp(dpent->name, name)) {
			return dpent;
		}
	}
	return NULL;
}

static int dd_param_fill(int argc, char **argv, struct dd_param *dp) {
	const struct dd_param_ent *dpent;
	int i;

	memset(dp, 0, sizeof(*dp));

	for (i = 1; i < argc; i++) {
		char *opt = argv[i];
		char *arg = strchr(opt, '=');

		if (!arg) {
			return -EINVAL;
		}

		*arg++ = '\0';

		dpent = dd_param_ent_find(opt);
		if (!dpent) {
			return -EINVAL;
		}

		dpent->type(dpent, dp, arg);
	}

	return 0;
}

static int dd_cond_open(const char *path, int mode, int def_fd) {
	int fd;

	if (path) {
		fd = open(path, mode, S_IRALL | S_IWALL);
		if (0 > fd) {
			return -errno;
		}
	} else {
		fd = def_fd;
	}
	return fd;
}

int main(int argc, char **argv) {
	struct dd_param dp = {0};
	void *tbuf;
	int ifd, ofd;
	int n_read, n_write, err;
	int format = 0;
	unsigned int addr = 0;

	err = dd_param_fill(argc, argv, &dp);
	if (err) {
		goto out;
	}

	if (!dp.bs) {
		dp.bs = DD_DEFAULT_BS;
	}

	ifd = dd_cond_open(dp.ifile, O_RDONLY, STDIN_FILENO);
	if (ifd < 0) {
		err = ifd;
		goto out;
	}

	if (!dp.count) {
		struct stat buff;

		err = fstat(ifd, &buff);
		if(err < 0) {
			goto out;
		}
		dp.count = buff.st_size;
	}

	ofd = dd_cond_open(dp.ofile, O_WRONLY | O_CREAT, STDOUT_FILENO);
	if (ofd < 0) {
		err = ofd;
		goto out_ifd_close;
	}

	tbuf = malloc(dp.bs);
	if (!tbuf) {
		err = -ENOMEM;
		goto out_ofd_close;
	}

	if((NULL != dp.format) && (0 == strcmp(dp.format, DD_FORMAT_HEX_C))) {
		format = 1;
	}

	while (dp.skip != 0 ) {
		n_read = read(ifd, tbuf, dp.bs);
		if (n_read < 0) {
			err = -errno;
			goto out_cmd;
		}
		if (n_read == 0) {
			goto out_cmd;
		}

		dp.skip --;
	} while (dp.skip != 0);

	do {
		n_read = read(ifd, tbuf, dp.bs);
		if (n_read < 0) {
			err = -errno;
			break;
		}

		if (n_read == 0) {
			break;
		}

		n_write = format ? write_stdout(tbuf, n_read, addr) :
				write(ofd, tbuf, n_read);
		if (0 > n_write) {
			err = -errno;
			break;
		}
		addr += n_read;
		dp.count --;
	} while (dp.count != 0);

out_cmd:
	free(tbuf);
out_ofd_close:
	close(ofd);
out_ifd_close:
	close(ifd);
out:
	return err;
}

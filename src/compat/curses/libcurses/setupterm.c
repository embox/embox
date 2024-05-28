/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mem/sysmalloc.h>
#include <terminfo/config.h>

#include "term_priv.h"

#define BUF_SZ          2048
#define MAGIC           0432

#define ENTRY_FOUND     1
#define ENTRY_NOT_FOUND 0
#define ERROR_OCCURRED  -1

#define RET_ERROR(ret_code, fmt, str) \
	if (errret) {                     \
		*errret = ret_code;           \
		goto out;                     \
	}                                 \
	fprintf(stderr, fmt, str);        \
	exit(EXIT_FAILURE)

int setupterm(char *term, int fildes, int *errret) {
	char filename[PATH_MAX];
	int fd;
	char buf[BUF_SZ];
	int16_t *header_section;
	int16_t name_size, flag_size, num_size;
	int16_t str_size, table_size, total_size;
	char *dst, *src;
	TERMINAL *termp;
	int rc = ERR;

	sprintf(filename, "%s/%c/%s", TERMINFO_PATH, term[0], term);

	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		RET_ERROR(ENTRY_NOT_FOUND, "Unknown terminal type `%s`\n", term);
	}

	if (-1 == read(fd, buf, sizeof(buf))) {
		close(fd);
		RET_ERROR(ERROR_OCCURRED, "%s\n", strerror(errno));
	}
	close(fd);

	header_section = (int16_t *)buf;
	if (header_section[0] != MAGIC) {
		RET_ERROR(ERROR_OCCURRED, "%s\n", "Invalid terminal entry format");
	}

	name_size = header_section[1];
	flag_size = header_section[2];
	num_size = header_section[3] * 2;
	str_size = header_section[4] * 2;
	table_size = header_section[5];

	if ((name_size < 0) || (flag_size < 0) || (num_size < 0) || (str_size < 0)
	    || (table_size < 0)) {
		RET_ERROR(ERROR_OCCURRED, "%s\n", "Invalid terminal entry format");
	}

	total_size = name_size + flag_size + num_size + str_size + table_size;

	termp = sysmalloc(sizeof(TERMINAL));
	if (termp == NULL) {
		RET_ERROR(ERROR_OCCURRED, "%s\n", "Cannot allocate memory");
	}

	dst = sysmalloc(total_size);
	if (dst == NULL) {
		free(termp);
		RET_ERROR(ERROR_OCCURRED, "%s\n", "Cannot allocate memory");
	}

	src = buf + 12;
	memcpy(dst, src, total_size);

	termp->type.term_names = dst;
	dst += name_size;

	termp->type.flags = (bool *)dst;
	dst += flag_size;

	termp->type.numbers = (int16_t *)dst;
	dst += num_size;

	termp->type.strings = (int16_t *)dst;
	dst += str_size;

	termp->type.str_table = dst;

	termp->fildes = fildes;

	set_curterm(termp);

	rc = OK;
	if (errret) {
		*errret = ENTRY_FOUND;
	}

out:
	return rc;
}

TERMINAL *set_curterm(TERMINAL *nterm) {
	TERMINAL *oterm;

	if (nterm == NULL) {
		return NULL;
	}

	oterm = cur_term;
	cur_term = nterm;

	return oterm;
}

int del_curterm(TERMINAL *oterm) {
	if (oterm == NULL) {
		return ERR;
	}

	sysfree(oterm->type.term_names);
	sysfree(oterm);

	return OK;
}

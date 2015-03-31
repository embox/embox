/**
 * @file
 * @brief
 *
 * @date 19.04.12
 * @author Bulychev Anton
 * @author Ilia Vaprol
 */

#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

#include <execinfo.h>

#include <lib/debug/symbol.h>

void backtrace_symbols_fd(void *const *buff, int size, int fd) {
	static char error_msg[] = "backtrace_symbols_fd error";
	int i;
	const struct symbol *s;
	ptrdiff_t offset;
	FILE *fp;

	fp = fdopen(fd, "w");
	if (fp == NULL) {
		write(fd, &error_msg[0], sizeof error_msg);
		return;
	}

	for (i = 0; i < size; ++i) {
		fprintf(fp, "%p", buff[i]);

		s = symbol_lookup(buff[i]);
		if (s != NULL) {
			offset = (char *)buff[i] - (char *)s->addr;
			fprintf(fp, " <%s+%#tx>",
					offset >= 0 ? s->name : "__unknown__",
					offset);
		}

		fprintf(fp, "\n");
	}

	fclose(fp);
}

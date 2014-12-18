/**
 * @file
 * @brief
 *
 * @date 19.04.12
 * @author Bulychev Anton
 * @author Ilia Vaprol
 */

#include <debug/symbol.h>
#include <execinfo.h>
#include <framework/mod/options.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/math.h>

#define MODOPS_ROW_COUNT OPTION_GET(NUMBER, row_count)
#define MODOPS_ROW_SIZE OPTION_GET(NUMBER, row_size)

static char bt_buff[MODOPS_ROW_COUNT][MODOPS_ROW_SIZE];

int backtrace_symbol_buf(void *ptr, char *out, int size) {
	const struct symbol *s;
	int res;

	s = symbol_lookup(ptr);
	if (s) {
		const ptrdiff_t offset = ptr - s->addr;
		const char *name = offset >= 0 ?  s->name : "__unknown__";

		res = snprintf(out, size, "%p <%s+%#tx>", ptr, name, offset);
	} else {
		res = snprintf(out, size, "%p", ptr);
	}

	return res;
}

char **backtrace_symbols(void *const *buff, int size) {
	char **out;

	out = malloc(size * sizeof(*out));
	if (out) {
		const int can_fill_symbols = min(size, MODOPS_ROW_COUNT);
		int i;

		for (i = 0; i < can_fill_symbols; ++i) {
			backtrace_symbol_buf(buff[i], bt_buff[i], sizeof(bt_buff[i]));
			out[i] = bt_buff[i];
		}

		for (i = can_fill_symbols; i < size; ++i) {
			out[i] = NULL;
		}
	}

	return out;
}

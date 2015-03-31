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
#include <stdlib.h>

#include <execinfo.h>

#include <util/math.h>

#include <debug/symbol.h>

#include <framework/mod/options.h>



#define MODOPS_ROW_COUNT OPTION_GET(NUMBER, row_count)
#define MODOPS_ROW_SIZE OPTION_GET(NUMBER, row_size)

static char bt_buff[MODOPS_ROW_COUNT][MODOPS_ROW_SIZE];

int backtrace_symbol_buf(void *ptr, char *out, int size) {
	const struct symbol *s;
	int res;

	s = symbol_lookup(ptr);
	if (s) {
		res = snprintf(out, size, "%p <%s+%#tx>",
				ptr, s->name, ptr - s->addr);
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

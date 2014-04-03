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

char ** backtrace_symbols(void *const *buff, int size) {
	int i;
	char **out;
	const struct symbol *s;
	ptrdiff_t offset;

	out = malloc(size * sizeof &bt_buff[0][0]);
	if (out == NULL) {
		return NULL;
	}

	for (i = 0; i < min(size, MODOPS_ROW_COUNT); ++i) {
		out[i] = &bt_buff[i][0];

		s = symbol_lookup(buff[i]);
		if (s != NULL) {
			offset = (char *)buff[i] - (char *)s->addr;
			snprintf(out[i], MODOPS_ROW_SIZE, "%p <%s+%#tx>",
					buff[i], offset >= 0 ? s->name : "__unknown__",
					offset);
		}
		else {
			snprintf(out[i], 30, "%p", buff[i]);
		}
	}

	while (i < size) {
		out[i++] = NULL;
	}

	return out;
}

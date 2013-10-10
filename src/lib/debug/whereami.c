/**
 * @file
 * @brief
 *
 * @date 10.10.13
 * @author Ilia Vaprol
 */

#include <debug/symbol.h>
#include <debug/whereami.h>
#include <execinfo.h>
#include <framework/mod/options.h>
#include <kernel/printk.h>
#include <util/array.h>
#include <util/math.h>

#define MODOPS_ROW_LIMIT OPTION_GET(NUMBER, row_limit)
#define MODOPS_MAX_DEPTH OPTION_GET(NUMBER, max_depth)

static void *bt_buff[MODOPS_MAX_DEPTH];

void whereami(void) {
	int i, size, limit;
	const struct symbol *s;
	ptrdiff_t offset;

	size = backtrace(&bt_buff[0], ARRAY_SIZE(bt_buff));
	limit = MODOPS_ROW_LIMIT == 0 ? size : min(size, MODOPS_ROW_LIMIT + 1);

	printk(
			"\n"
			"\n"
			"Backtrace:\n"
			"    pc         func + offset\n"
			"    ---------- ------------------------\n"
			);

	for (i = 1; i < limit; ++i) {
		printk("%3d %p", size - i, bt_buff[i]);

		s = symbol_lookup(bt_buff[i]);
		if (s != NULL) {
			offset = (char *)bt_buff[i] - (char *)s->addr;
			printk(" <%s+%#tx>", offset >= 0 ? s->name : "__unknown__",
					offset);
		}
		printk("\n");
	}
}

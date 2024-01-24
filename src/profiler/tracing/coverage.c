/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.02.2014
 */

#include <stdbool.h>
#include <errno.h>
#include <debug/symbol.h>
#include <lib/libds/bitmap.h>
#include <kernel/printk.h>
#include <embox/unit.h>
#include "cyg_profile.h"

extern const struct symbol __symbol_table[];
extern const size_t __symbol_table_size;

EMBOX_UNIT_INIT(coverage_init);

#define COVERAGE_TABLE_SIZE OPTION_GET(NUMBER,coverage_table_size)
static BITMAP_DECL(coverage_table_bitmap, COVERAGE_TABLE_SIZE);

void __coverage_func_enter(void *func, void *caller) {
	const struct symbol *sym;
	int sym_pos;

	sym = symbol_lookup(func);
	if (!sym)
		return;

	sym_pos = sym - __symbol_table;
	if (!(0 <= sym_pos && sym_pos < __symbol_table_size))
		return;

	bitmap_set_bit(coverage_table_bitmap, sym_pos);
}

void __coverage_func_exit(void *func, void *caller) {

}

int coverage_getstat(const struct symbol **sym_table, const unsigned long **cov_bitmap) {

	*sym_table = __symbol_table;
	*cov_bitmap = coverage_table_bitmap;

	return __symbol_table_size;
}

static int coverage_init(void) {

	if (COVERAGE_TABLE_SIZE < __symbol_table_size) {
		printk("\n[%s]: coverage_table_size too small, "
				"try to increase it [%zu; required at least %zu]\n",
				__func__, (size_t)COVERAGE_TABLE_SIZE, __symbol_table_size);
		return -ENOMEM;
	}


	ARRAY_SPREAD_DECLARE(cyg_func, __cyg_handler_enter_array);
	ARRAY_SPREAD_DECLARE(cyg_func, __cyg_handler_exit_array);
	ARRAY_SPREAD_ADD(__cyg_handler_enter_array, &__coverage_func_enter);
	ARRAY_SPREAD_ADD(__cyg_handler_exit_array, &__coverage_func_exit);

	return 0;
}

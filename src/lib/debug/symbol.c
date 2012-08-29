/**
 * @file
 * @brief TODO
 *
 * @date Aug 28, 2012
 * @author Eldar Abusalimov
 */

#include <debug/symbol.h>
#include <types.h>

extern const struct symbol __symbol_table[];
extern const size_t __symbol_table_size;

const struct symbol *symbol_lookup(void *addr) {
	/* Binary search */
	size_t l = 0, r = __symbol_table_size - 1, m;

	if (__symbol_table[0].addr > addr) {
		return NULL;
	}

	while (l < r) {
		m = (l + r + 1) / 2;
		if (__symbol_table[m].addr <= addr) {
			l = m;
		} else {
			r = m - 1;
		}
	}

	return &__symbol_table[l];
}


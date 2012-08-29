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
	size_t l = 0, r = __symbol_table_size, m;

	/* Binary search */
	while (l < r) {
		m = (l + r + 1) / 2;
		if (__symbol_table[m].addr <= addr) {
			l = m;
		} else {
			r = m - 1;
		}
	}

	if (l == __symbol_table_size) {
		return NULL;
	} else {
		return &__symbol_table[l];
	}
}


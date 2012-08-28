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
	const struct symbol *last = NULL;

	/* Plain search. */
	for (const struct symbol *s = __symbol_table;
			s < __symbol_table + __symbol_table_size; ++s) {

		if (s->addr > addr) {
			break;
		}
		last = s;
	}

	return last;
}


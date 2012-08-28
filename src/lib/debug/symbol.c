/**
 * @file
 * @brief TODO
 *
 * @date Aug 28, 2012
 * @author Eldar Abusalimov
 */

#include <debug/symbol.h>
#include <types.h>

/* {NULL,NULL}-terminated array of symbol structs. */
extern const struct symbol __symbol_table[];

const struct symbol *symbol_lookup(void *addr) {
	/* Simplest implementation */
	int k = 0;
	while (__symbol_table[k].addr != NULL
		|| __symbol_table[k].name != NULL) {

		if (__symbol_table[k].addr <= addr) {
			return &__symbol_table[k];
		}
		k++;
	}
	return NULL;
}


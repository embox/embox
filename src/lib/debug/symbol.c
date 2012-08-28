/**
 * @file
 * @brief TODO
 *
 * @date Aug 28, 2012
 * @author Eldar Abusalimov
 */

#include <debug/symbol.h>

/* {NULL,NULL}-terminated array of symbol structs. */
extern const struct symbol __symbol_table[];

const struct symbol *symbol_lookup(void *addr) {
	// TODO NIY.
	return NULL;
}


/**
 * @file
 * @brief TODO
 *
 * @date Aug 28, 2012
 * @author Eldar Abusalimov
 */

#ifndef DEBUG_SYMBOL_H_
#define DEBUG_SYMBOL_H_

struct symbol {
	void *addr;
	const char *name;
};

/**
 * Searches for a symbol with an address closest to the given one, but
 * not greater than it.
 *
 * @param addr
 *   The address.
 * @return
 *   A symbol if any has been found, or @c NULL otherwise.
 */
extern const struct symbol *symbol_lookup(void *addr);

#endif /* DEBUG_SYMBOL_H_ */

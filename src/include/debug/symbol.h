/**
 * @file
 * @brief
 *
 * @date 28.08.12
 * @author Eldar Abusalimov
 */

#ifndef LIB_EXECINFO_SYMBOL_H_
#define LIB_EXECINFO_SYMBOL_H_

#include <util/location.h>

struct symbol {
	void *addr;
	const char *name;
	struct location loc;
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

#endif /* LIB_EXECINFO_SYMBOL_H_ */

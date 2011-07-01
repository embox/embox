/**
 * @brief Adapt pool structure to objalloc interface.
 *
 * @see mem/objalloc.h
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#include <mem/slab.h>

struct objalloc {
	struct cache cache;
};

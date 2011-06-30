/**
 * @brief Adapt pool structure to objalloc interface.
 *
 * @see kernel/mm/objalloc.h
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#include <kernel/mm/slab.h>

struct objalloc {
	struct cache cache;
};

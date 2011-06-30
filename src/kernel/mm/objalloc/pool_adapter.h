/**
 * @brief Adapt cache structure to objalloc interface.
 *
 * @see kernel/mm/objalloc.h
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#include <kernel/mm/misc/pool.h>

struct adapter {
	struct pool pool;
};


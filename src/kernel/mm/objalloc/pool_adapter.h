/**
 * @brief Adapt cache structure to objalloc interface.
 *
 * @see kernel/mm/objalloc.h
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#include <util/pool.h>

struct adapter {
	struct pool pool;
};

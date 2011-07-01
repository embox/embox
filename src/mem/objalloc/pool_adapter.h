/**
 * @brief Adapt cache structure to objalloc interface.
 *
 * @see mem/objalloc.h
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#include <mem/misc/pool.h>

struct adapter {
	struct pool pool;
};


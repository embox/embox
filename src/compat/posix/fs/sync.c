/**
 * @file
 * @brief Causes all buffered modifications to file metadata and data
 *        to be written to the underlying file systems, i.e. flushes all buffer cache.
 *
 * @author  Alexander Kalmuk
 * @date    2.08.2013
 */

#include <unistd.h>
#include <fs/bcache.h>

void sync(void) {
	/*
	 * TODO higher-level I/O layers such as stdio may maintain separate buffers of their own.
	 * So we should flush those too, not only buffer cache. --Alexander
	 */
	bcache_flush_all();
}

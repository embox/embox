/**
 * @file
 *
 * @date 25.12.2009
 * @author Anton Bondarev
 */

#ifndef CACHE_H_
#define CACHE_H_

#include <asm/cache.h>
 #ifndef ____cacheline_aligned
 #define ____cacheline_aligned __attribute__((__aligned__(CACHE_LINE_LENGTH)))
 #endif

#endif /* CACHE_H_ */

/**
 * @file
 *
 * @brief Time types
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_TIME_TIME_TYPES_H_
#define KERNEL_TIME_TIME_TYPES_H_

#include <stdint.h>
//include posix types

#define MSEC_PER_SEC    1000L
#define USEC_PER_MSEC   1000L
#define NSEC_PER_USEC   1000L
#define USEC_PER_SEC    1000000L
#define NSEC_PER_SEC    1000000000L

typedef uint64_t cycle_t;
typedef uint64_t ns_t;
typedef uint64_t ms_t;

#endif /* KERNEL_TIME_TIME_TYPES_H_ */

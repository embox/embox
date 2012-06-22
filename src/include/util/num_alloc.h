/**
 * @file
 * @brief Interface for number alloc struct. Structure has requests:
 *  * Alloc number -- returns minimal free number and mark it as busy.
 *  * Free number -- mark number as free
 * Optionaly, it can handle with range arguments:
 *  * Alloc number range -- returns range of numbers, all are free
 *  * Free range -- mark range as free
 *
 * @author  Anton Kozlov
 * @date    21.06.2012
 */

#ifndef UTIL_NUM_ALLOC_H_
#define UTIL_NUM_ALLOC_H_

#include <types.h>

#include <module/embox/util/num_alloc.h>

/**
 * @brief Abstract realization
 */
typedef __util_num_alloc_t util_num_alloc_t;

#define UTIL_NUM_ALLOC_DEF(name, n) __UTIL_NUM_ALLOC_DEF(name, n)

#define UTIL_NUM_ALLOC_DEF_INLINE(name, n) __UTIL_NUM_ALLOC_DEF_INLINE(name, n)

/**
 * @brief Calculate size of space, needed for num_alloc struct that will
 * manage at most n numbers
 *
 * @param n Required number capacity for num_alloc
 *
 * @return
 */
#define UTIL_NUM_ALLOC_CALC(n) __UTIL_NUM_ALLOC_CALC(n)

/**
 * @brief Initialize space for num_alloc struct
 *
 * @param space Pointer for space which can store struct with capacity n
 * @param n Capacity of initializing num_alloc
 *
 * @return Pointer for initilized num_alloc
 */
util_num_alloc_t *util_num_alloc_init(void *space, int n);

/**
 * @brief Allocate a positive integer from num_alloc
 *
 * @param num_alloc Pointer for util_num_alloc_t
 *
 * @return Minimal free integer, if there. Negative error code, otherwise.
 */
int util_num_alloc(util_num_alloc_t *num_alloc);

/**
 * @brief Allocata specified number
 *
 * @param num_alloc
 * @param n
 *
 * @return n on success, negative otherwise
 */
int util_num_alloc_specified(util_num_alloc_t *num_alloc, int n);

/**
 * @brief Mark number as free in num_alloc
 *
 * @param num_alloc Pointer for util_num_alloc_t
 * @param n Number which are freeing
 */
void util_num_free(util_num_alloc_t *num_alloc, int n);

/**
 * @brief Return a number that will be allocated on next allocation request
 *
 * @param num_alloc
 *
 * @return
 */
int util_num_alloc_next_alloc(util_num_alloc_t *num_alloc);

/**
 * @brief Test if a number is free or allocated
 *
 * @param num_alloc
 * @param n
 *
 * @return 0 on free, otherwise on allocated
 */
int util_num_alloc_test(util_num_alloc_t *num_alloc, int n);

/* Optional */

int util_num_alloc_range(util_num_alloc_t *num_alloc, int len);

int util_num_free_range(util_num_alloc_t *num_alloc, int n, int len);

#endif /* UTIL_NUM_ALLOC_H_ */

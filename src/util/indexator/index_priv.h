/**
 * @file
 * @brief
 *
 * @date 09.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <stddef.h>

#include <util/indexator.h>

extern int ind_check(struct indexator *ind, size_t idx);

extern int ind_get_bit(struct indexator *ind, size_t idx);

extern void ind_set_bit(struct indexator *ind, size_t idx);

extern void ind_unset_bit(struct indexator *ind, size_t idx);

extern size_t ind_find_rand(struct indexator *ind);

extern size_t ind_find_less(struct indexator *ind, size_t idx, size_t min,
    size_t none);

extern size_t ind_find_more(struct indexator *ind, size_t idx, size_t max,
    size_t none);

/**
 * @file
 * @brief TODO
 *
 * @date 10.03.10
 * @author Eldar Abusalimov
 */

#ifndef EMBOX_RUNLEVEL_H_
#define EMBOX_RUNLEVEL_H_

/** Total amount of run levels. */
#define RUNLEVEL_NRS_TOTAL 5

/**
 * Checks if the specified @c runlevel_nr is less then #RUNLEVEL_NRS_TOTAL
 * value.
 *
 * @param runlevel_nr the number to check
 * @return comparison result
 */
#define runlevel_nr_valid(runlevel_nr) \
	(0 <= (runlevel_nr_t) runlevel_nr \
	&& (runlevel_nr_t) runlevel_nr < (runlevel_nr_t) RUNLEVEL_NRS_TOTAL)

/** Type representing the run level. */
typedef int runlevel_nr_t;

/**
 * Sets the run level to the exact value.
 *
 * @param level the new run level
 * @return operation result
 * @retval 0 on success
 * @retval nonzero TODO
 */
extern int runlevel_set(runlevel_nr_t level);

#endif /* EMBOX_RUNLEVEL_H_ */

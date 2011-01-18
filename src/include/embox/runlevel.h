/**
 * @file
 * @brief TODO
 *
 * @date 10.03.2010
 * @author Eldar Abusalimov
 */

#ifndef EMBOX_RUNLEVEL_H_
#define EMBOX_RUNLEVEL_H_

/** Total amount of run levels. */
#define RUNLEVEL_NRS_TOTAL 4

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

extern int runlevel_enter(runlevel_nr_t level);
extern int runlevel_leave(runlevel_nr_t level);

extern runlevel_nr_t runlevel_get_entered(void);
extern runlevel_nr_t runlevel_get_leaved(void);

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

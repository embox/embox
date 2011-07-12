/**
 * @brief API for locks functions.
 *
 * @date 12.07.11
 * @author Alexandr Kalmuk
 */

/**
 * Locks hardirq and to come in critical section.
 *
 * When hardirq locked do not call softirq_dispatch, sched_dispatch.
 * This function will be called after hardirq_unlock().
 *
 * Each lock must be balanced with the corresponding unlock.
 * @param critical
 */
extern void irq_lock(__critical_t critical);

/**
 * Unlock hardirq and to came out from critical section.
 * Must be called on the previously locked irq only
 *
 * @see irq_lock()
 */
extern void irq_unlock(__critical_t critical);

/**
 * Locks hardirq and to come in critical section.
 *
 * When hardirq locked do not call sched_dispatch().
 * sched_dispatch() will be called after hardirq_unlock().
 *
 * Each lock must be balanced with the corresponding unlock.
 *
 * @see softirq_lock()
 */
extern void softirq_lock(__critical_t critical);

/**
 * Unlocks hardirq and to came out from critical section
 * Must be called on the previously locked softirq only.
 *
 * @see softirq_lock()
 */
extern void softirq_unlock(__critical_t critical);

/**
 * Locks the scheduler which means disabling thread switch until
 * #sched_unlock() is called. Each lock must be balanced with the corresponding
 * unlock.
 *
 * @see sched_unlock()
 */
extern void sched_lock(void);

/**
 * Unlocks the scheduler after #sched_lock(). Must be called on the
 * previously locked scheduler only. Outermost unlock (which corresponds to
 * the first lock call) also dispatches pending thread switches (if any).
 *
 * @see sched_lock()
 */
extern void sched_unlock(void);

/**
 * @file
 * @brief Declares critical API
 *
 * @date 16.05.2010
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_H_
#define KERNEL_CRITICAL_H_

#include __impl(kernel/critical.h)

#define CRITICAL_HARDIRQ __CRITICAL_HARDIRQ
#define CRITICAL_SOFTIRQ __CRITICAL_SOFTIRQ
#define CRITICAL_PREEMPT __CRITICAL_PREEMPT

typedef __critical_t critical_t;

extern int critical_allows(critical_t critical);

extern int critical_inside(critical_t critical);

extern void critical_enter(critical_t critical);

extern void critical_leave(critical_t critical);

// TODO remove everything below. -- Eldar

inline static unsigned int critical_inside_hardirq(void) {
	return critical_inside(CRITICAL_HARDIRQ);
}

inline static void critical_enter_hardirq(void) {
	critical_enter(CRITICAL_HARDIRQ);
}

inline static void critical_leave_hardirq(void) {
	critical_leave(CRITICAL_HARDIRQ);
}

inline static unsigned int critical_allows_softirq(void) {
	return critical_allows(CRITICAL_SOFTIRQ);
}

inline static unsigned int critical_inside_softirq(void) {
	return critical_inside(CRITICAL_SOFTIRQ);
}

inline static void critical_enter_softirq(void) {
	critical_enter(CRITICAL_SOFTIRQ);
}

inline static void critical_leave_softirq(void) {
	critical_leave(CRITICAL_SOFTIRQ);
}

inline static unsigned int critical_allows_preempt(void) {
	return critical_allows(CRITICAL_PREEMPT);
}

inline static unsigned int critical_inside_preempt(void) {
	return critical_inside(CRITICAL_PREEMPT);
}

inline static void critical_enter_preempt(void) {
	critical_enter(CRITICAL_PREEMPT);
}

inline static void critical_leave_preempt(void) {
	critical_leave(CRITICAL_PREEMPT);
}

#endif /* KERNEL_CRITICAL_H_ */

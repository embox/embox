/**
 * @file
 * @brief Pinning a thread from a test case.
 */

#ifndef FRAMEWORK_TEST_THREAD_PIN_H_
#define FRAMEWORK_TEST_THREAD_PIN_H_

#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>
#include <kernel/thread.h>

/**
 * Binds a thread to the core the suite runs on, so a case that asserts an
 * order sees the threads take turns. A case measuring spread must not pin.
 * No-op without SMP.
 */
static inline void test_thread_pin(struct thread *t) {
#ifdef SMP
	cpu_bind(cpu_get_id(), t);
#else
	(void)t;
#endif
}

#endif /* FRAMEWORK_TEST_THREAD_PIN_H_ */

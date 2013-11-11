/**
 * @file
 *
 * @brief Defines framework for thread states used for thread state machine.
 *
 * @date 24.04.11
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_STATE_H_
#define KERNEL_THREAD_STATE_H_

/* thread state in scheduler state machine */
#define __THREAD_STATE_READY     (0x1 << 0) /* present in runq */
/* present in waitq or waiting for lunching*/
#define __THREAD_STATE_WAITING   (0x1 << 1)
#define __THREAD_STATE_RUNNING   (0x1 << 2) /* executing on CPU now */
#define __THREAD_STATE_EXITED    (0x1 << 3) /* zombie */

#endif /* KERNEL_THREAD_STATE_H_ */

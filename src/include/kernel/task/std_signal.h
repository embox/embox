/**
 * @file
 * @brief Functions to operate with standard signals
 *
 * @author  Anton Kozlov
 * @date    20.06.2012
 */

#include "signal.h"

#ifndef SRC_INCLUDE_KERNEL_TASK_SIGNAL_H_
#define SRC_INCLUDE_KERNEL_TASK_SIGNAL_H_

extern void task_stdsig_send(struct task *task, int sig);
extern void task_stdsig_handle(void);

#endif /* SRC_INCLUDE_KERNEL_TASK_SIGNAL_H_ */

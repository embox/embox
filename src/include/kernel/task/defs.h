/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.04.2014
 */

#ifndef KERNEL_TASK_DEFS_H_
#define KERNEL_TASK_DEFS_H_

#define TASKST_EXITED_MASK      0x0100
#define TASKST_EXITST_MASK      0x00ff
#define TASKST_SIGNALD_MASK     0x0200
#define TASKST_TERMSIG_MASK     TASKST_EXITST_MASK
#define TASKST_STOPPED_MASK     0x0400
#define TASKST_STOPSIG_MASK     TASKST_EXITST_MASK
#define TASKST_CONT_MASK        0x0800


#define TASK_STATUS_IN_VFORK    0x1000


#endif /* KERNEL_TASK_DEFS_H_ */


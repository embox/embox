
/**
 * @file
 * @brief Declaration of structure of addreses for dropbear session structure
 * @date 24.07.2023
 */

#ifndef DROPBEAR_RES_H_
#define DROPBEAR_RES_H_

#include <kernel/task.h>
#include "stdint.h"


struct drpbr_ses{
    uintptr_t ses_struct_addr[4]; /* array for storing adresses of 4 global
                                session structures for each particular
                                dropbear session */
};


extern struct drpbr_ses *task_resource_dropbear(const struct task *task);
extern struct drpbr_ses *task_self_resource_dropbear(void);

#endif

/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.12.2013
 */

#ifndef ARCH_USERMODE_CTX_H_
#define ARCH_USERMODE_CTX_H_

#include <kernel/host.h>

struct context {
	char ucp[HOST_CTX_LEN];
};

#endif /* ARCH_USERMODE_CTX_H_ */


/**
 * @file
 * @brief
 *
 * @date 21.11.12
 * @author Anton Bulychev
 */

#include <asm/entry.h>

	.global fork_leave
fork_leave:
	mov 4(%esp), %esp
	RESTORE_ALL

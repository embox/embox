/**
 * @file
 * @brief
 *
 * @date 25.07.2013
 * @author Andrey Gazukin
 */

#ifndef __LINUX_WAIT_H_
#define __LINUX_WAIT_H_

#include <sys/wait.h>

typedef struct { } wait_queue_head_t;

#define init_waitqueue_head(wait) do{} while (0)
#define add_wait_queue(wait,new_wait) do{} while (0)
#define remove_wait_queue(wait,old_wait) do{} while (0)
#define DECLARE_WAITQUEUE(wait,current) do{} while (0)

static inline void wake_up(wait_queue_head_t *erase_wait)
{ /* Only used for waking up threads blocks on erases. Not used in eCos */ }

#endif /* WAIT_H_ */

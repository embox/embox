/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    29.05.2014
 */

#ifndef SCHED_WAITQ_PROTECT_LINK_H_
#define SCHED_WAITQ_PROTECT_LINK_H_

extern struct waitq_link *waitq_link_create_protected(struct waitq_link *uwql);
extern struct waitq_link *waitq_link_find_protected(struct waitq_link *pwql);
extern void waitq_link_delete_protected(struct waitq_link *pwql);

#include <module/embox/kernel/sched/waitq_protect_link.h>

#endif /* SCHED_WAITQ_PROTECT_LINK_H_ */



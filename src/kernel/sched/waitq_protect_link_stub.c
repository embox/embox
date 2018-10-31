/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    04.06.2014
 */

#include <kernel/sched/waitq_protect_link.h>

struct waitq_link *waitq_link_create_protected(struct waitq_link *uwql) {
	return uwql;
}

struct waitq_link *waitq_link_find_protected(struct waitq_link *uwql) {
	return uwql;
}

void waitq_link_delete_protected(struct waitq_link *pwql) {

}


/**
 * @file
 * @brief TODO
 *
 * @date 16.02.2010
 * @author Eldar Abusalimov
 */

#include <errno.h>

#include <kernel/softirq.h>

int softirq_install(softirq_nr_t nr, softirq_handler_t handler,
		void *dev_id){
	return -ENOSYS;
}

int softirq_raise(softirq_nr_t nr){
	return -ENOSYS;
}

void softirq_dispatch(void){
}

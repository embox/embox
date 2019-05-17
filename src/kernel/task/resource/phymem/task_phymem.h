/**
 * @file phymem.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 15.05.2019
 */

#ifndef TASK_PHYMEM_H_
#define TASK_PHYMEM_H_

extern int task_resource_phymem_add(const struct task *task, void *paddr, int pages);

#endif /* TASK_PHYMEM_H_ */

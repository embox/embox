/**
 * @file
 * @brief
 *
 * @author  Anton Ostrouhhov
 * @date    03.05.2022
 */

#ifndef TASK_LKL_RESOURCES_H_
#define TASK_LKL_RESOURCES_H_

/* This is to keep TLS key (pthread_key) for LKL */
struct lkl_tls_struct {
	size_t pthread_key;
};

struct lkl_resources {
	int lkl_allowed;
	struct lkl_tls_struct *lkl_tls_key;
};

extern struct lkl_resources *task_lkl_resources(const struct task *task);

#endif /* TASK_LKL_RESOURCES_H_ */
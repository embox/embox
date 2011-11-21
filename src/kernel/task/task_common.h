/**
 * @file
 * @brief
 *
 * @date 16.09.11
 * @author Anton Kozlov
 */

#include <kernel/task.h>

void task_root_init(struct task *new_task);

void fd_list_init(struct task_resources *res);

int desc2idx(struct __fd_list *desc, struct task_resources *res);

struct __fd_list *task_fdl_alloc(struct task_resources *res);

int task_fdl_free(struct __fd_list *fdl, struct task_resources *res);

/*
 * Internal function that assign fd with file in task
 * @param fd Integer representing file
 * @param file Stream to be assigned
 * @param tsk Task
 * @return
 */
int __file_opened_fd(int fd, FILE *file, struct task_resources *res);


/**
 * @file
 * @brief
 *
 * @date 16.09.11
 * @author Anton Kozlov
 */

#include <kernel/task.h>

void task_root_init(struct task *new_task);

void fd_list_init(struct __fd_array *fd_array);
/**
 * Internal function that assign fd with file in task
 * @param fd Integer representing file
 * @param file Stream to be assigned
 * @param tsk Task
 * @return
 */
extern int __file_opened_fd(int fd, FILE *file, struct task *tsk);


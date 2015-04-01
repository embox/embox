/**
 * @file
 * @brief
 *
 * @date 28 Mar 2015
 * @author Denis Deryugin
 */

#ifndef FILE_TABLE_H_
#define FILE_TABLE_H

#define FILE_TABLE_SZ 16

struct file_table {
	struct dentry *root;
	struct dentry *pwd;
	struct file *file[FILE_TABLE_SZ];
};

extern struct file_table *task_resource_file_table(const struct task *task);

extern struct task* task_self(void);
static inline struct file_table *task_fs(void) {
	return task_resource_file_table(task_self());
}
#endif /* FILE_TABLE_H_ */

/**
 * @file
 * @brief
 *
 * @date 17.04.11
 * @author Anton Bondarev
 */

#ifndef TASK_H_
#define TASK_H_

#include <fs/file.h>
#include <lib/list.h>

struct __fd_list {
	struct list_head link; /**< link in opened/free lists. */
	FILE *file; /** FILE*, for which this fd is corresponding. */
	int type;
	//char unchar;
	//int err;
};

//TODO rewrite it
struct idx_desc {
	struct socket *socket;
};

#define CONFIG_TASKS_FILE_QUANTITY 16

struct task_resources {
	int fds_cnt;
	int file_idx_cnt;
	int socket_idx_cnt;
#if 0
	struct list_head fds_free;
	struct list_head fds_opened;
#endif
	struct __fd_list fds[CONFIG_TASKS_FILE_QUANTITY];
	struct idx_desc socket_fds[CONFIG_TASKS_FILE_QUANTITY];
};

struct task {
	struct task *parent;

	struct list_head children;
	struct list_head link;

	struct list_head threads;

	struct task_resources resources;

	int errno;
};

static inline struct task_resources *task_get_resources(struct task *task) {
	return &task->resources;
}
extern int task_create(struct task **new, struct task *parent);

extern struct task *task_self(void);

extern struct task *task_default_get(void);

extern int task_file_open(FILE *file, struct task_resources *res);

enum {
	TASK_IDX_TYPE_FILE = 0, TASK_IDX_TYPE_SOCKET = 1
};

extern int task_idx_alloc(int type);
extern int task_idx_release(int idx);

extern int task_idx_to_type(int idx);

extern int task_idx_save(int idx, void *desc);

extern void * task_idx_to_desc(int idx);

static inline int task_valid_fd(int fd) {
	return 0 <= fd && fd <= CONFIG_TASKS_FILE_QUANTITY;
}

#endif /* TASK_H_ */

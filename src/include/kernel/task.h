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
	struct list_head file_link; /**< link in list of all fds that corresponding for this FILE*. */
	FILE *file; /** FILE*, for which this fd is corresponding. */
	//char unchar;
	//int err;
};

//TODO rewrite it
struct idx_desc {
	struct socket *socket;
};

#define FD_N_MAX 16

struct __fd_array {
	struct list_head free_fds;
	struct list_head opened_fds;
	struct __fd_list fds[FD_N_MAX];
	struct idx_desc socket_fds[FD_N_MAX];
};

struct resources_manager {
	int file_idx_cnt;
	int sock_idx_cnt;
};

typedef struct task {
	struct task *parent;

	struct list_head children;
	struct list_head link;

	struct list_head threads;

	/* files */
	struct __fd_array fd_array;

	struct resources_manager rc_manager;

	int errno;
} task_t;

extern int task_create(struct task **new, struct task *parent);

extern struct task *task_self(void);

extern struct task *task_default_get(void);

extern int task_file_open(FILE *file, struct task *tsk);

enum {
	TASK_IDX_TYPE_FILE = 0, TASK_IDX_TYPE_SOCKET = 1
};

extern int task_idx_alloc(int type);
extern int task_idx_release(int idx);

extern int task_idx_to_type(int idx);

extern int task_idx_save(int idx, void *desc);

extern void * task_idx_to_desc(int idx);

#endif /* TASK_H_ */

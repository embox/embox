/**
 * @file
 * @brief
 *
 * @date 26.11.11
 * @author Anton Kozlov
 */

#ifndef TASK_RESOURCES_H_
#define TASK_RESOURCES_H_

enum {
	TASK_IDX_TYPE_FILE = 0,
	TASK_IDX_TYPE_SOCKET = 1
};


struct task_res_ops {
	int	type;
	int	(*close)(int idx);
	ssize_t (*read) (int fd, const void *buf, size_t nbyte);
	ssize_t (*write)(int fd, const void *buf, size_t nbyte);
};

struct __fd_list {
	struct list_head link; /**< link in opened/free lists. */
	FILE *file; /** FILE*, for which this fd is corresponding. */
	int type;
};

//TODO rewrite it
struct idx_desc {
	struct socket *socket;
};

#define CONFIG_TASKS_FILE_QUANTITY 16

struct task_resources {
#if 0
	int fds_cnt;
	int file_idx_cnt;
	int socket_idx_cnt;
	struct list_head fds_free;
	struct list_head fds_opened;
	struct idx_desc socket_fds[CONFIG_TASKS_FILE_QUANTITY];
#endif
	struct __fd_list fds[CONFIG_TASKS_FILE_QUANTITY];
};

#endif


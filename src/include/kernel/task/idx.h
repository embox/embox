/**
 * @file
 * @brief
 *
 * @date 15.06.12
 * @author Anton Kozlov
 */

#ifndef TASK_IDX_H_
#define TASK_IDX_H_

#include <framework/mod/options.h>
#include <module/embox/kernel/task/idx_table.h>

#define TASKS_RES_QUANTITY OPTION_MODULE_GET(embox__kernel__task__idx_table,NUMBER,task_res_quantity)

#include <types.h>
#include <assert.h>
#include <stdarg.h>

struct event;

enum task_idx_ops_type {
	TASK_RES_OPS_REGULAR,
	TASK_RES_OPS_TTY
};

typedef unsigned int idx_flags_t;

struct task_idx_ops;

struct idx_io_op_state {
	struct event *unblock;
	bool can_perform_op;
};

struct idx_desc_data {
	const struct task_idx_ops *res_ops;
	int link_count; /**< @brief Count of links in all tasks */
	void *fd_struct;     /**< @brief Pointer for actual struct */
	struct idx_io_op_state read_state;
	struct idx_io_op_state write_state;
};

/**
 * Descriptor for numerated item for resource (int flides / int socket)
 * Task stores resources in this form, dynamically resolve operations
 * with resources by type
 */
struct idx_desc {
	struct idx_desc_data *data;
	idx_flags_t flags;
};

/**
 * Specify operations with task's resources, which be called POSIX compat lib
 */
struct task_idx_ops {
	int	(*read) (struct idx_desc *data, void *buf, size_t nbyte);
	int	(*write)(struct idx_desc *data, const void *buf, size_t nbyte);
	int	(*close)(struct idx_desc *data);
	int	(*ioctl)(struct idx_desc *data, int request, va_list args);
	int	(*fcntl)(struct idx_desc *data, int cmd, va_list args);
	int	(*fseek)(struct idx_desc *data, long int offset, int origin);
	int	(*fstat)(struct idx_desc *data, void *buff);
	const enum task_idx_ops_type type;
};

/**
 * @brief idx utillity: get resource by idx
 * @param desc idx descriptor to get
 * @return resource in idx
 */
static inline void *task_idx_desc_data(struct idx_desc *desc) {
	assert(desc);
	return desc->data->fd_struct;
}

static inline idx_flags_t *task_idx_desc_flags_ptr(struct idx_desc *desc) {
	return &desc->flags;
}

static inline const struct task_idx_ops *task_idx_desc_ops(struct idx_desc *desc) {
	assert(desc);
	return desc->data->res_ops;
}

#if 0
/**
 * @brief Allocate idx descriptor structure with type and data
 *
 * @param type idx descriptor type
 * @param data pointer for idx descriptor data
 *
 * @return
 */
extern struct idx_desc *task_idx_desc_alloc(const struct task_idx_ops *ops, void *data);
#endif
#if 0
/**
 * @brief idx utillity: get reference count by idx
 * @param desc idx descriptor to get
 * @return count of references to idx resource
 */
static inline int task_idx_desc_link_count(struct idx_desc *desc) {
	assert(desc);
	return desc->data->link_count;
}

/**
 * @brief idx utillity: update reference count by idx
 * @param desc idx descriptor to update
 * @param d references count delta
 * @return new count of references to idx resource
 */
static inline int task_idx_desc_link_count_add(struct idx_desc *desc, int d) {
	assert(desc);
	return (desc->data->link_count += d);
}
#endif

#include <util/idx_table.h>

struct task_idx_table {
	UTIL_IDX_TABLE_DEF_INLINE(struct idx_desc *, idx, TASKS_RES_QUANTITY);
};


/**
 * @brief Get idx descriptor from task resources by idx number
 *
 * @param res Task resources to get idx from
 * @param idx idx number
 *
 * @return Pointer to idx
 */
static inline struct idx_desc *task_idx_table_get(struct task_idx_table *res, int idx) {
	assert(res);
	return (struct idx_desc *) util_idx_table_get((util_idx_table_t *) &res->idx, idx);
}

/**
 * @brief Set task resource's 'number' resource to idx
 *
 * @param res Task resource
 * @param idx number to set
 * @param desc idx descriptor to store in task resources
 */
extern int task_idx_table_set(struct task_idx_table *res, int idx, struct idx_desc *desc);

extern int task_idx_table_first_unbinded(struct task_idx_table *res);

/**
 * @brief Check if task resource number is occupied
 *
 * @param res Task resource to check
 * @param idx Number to check
 *
 * @return
 */
static inline int task_idx_table_is_binded(struct task_idx_table *res, int idx) {
	assert(res);
	return task_idx_table_get(res, idx) != NULL;
}

/**
 * @brief Mark task resource's number as free. Use with care, old resource should
 * be properly freed
 *
 * @param res Task resource to deal with
 * @param idx idx number
 */
static inline int task_idx_table_unbind(struct task_idx_table *res, int idx) {
	return task_idx_table_set(res, idx, NULL);
}

struct task;
extern struct task *task_self(void);
static inline struct task_idx_table *task_idx_table(struct task *task);

/**
 * @brief Determ is given fd is valid to use with tasks
 * @param fd File descriptor number to test
 * @return If given fs is valid to use with tasks
 */
static inline int task_valid_fd(int fd) {
	return 0 <= fd && fd <= TASKS_RES_QUANTITY;
}

/**
 * @brief Get task resources of self task
 *
 * @return Task resources of self task
 */
static inline struct task_idx_table *task_self_idx_table(void) {
	return task_idx_table(task_self());
}

static inline struct idx_desc *task_self_idx_get(int fd) {
	struct task_idx_table *res = task_self_idx_table();
	assert(res);
	assert(task_valid_fd(fd));
	return task_idx_table_get(task_self_idx_table(), fd);
}

/**
 * @brief Set idx descriptor of self task
 *
 * @param fd idx descriptor number
 * @param desc idx descriptor pointer to associate with number
 */
static inline int task_self_idx_set(int fd, struct idx_desc *desc) {
	return task_idx_table_set(task_self_idx_table(), fd, desc);
}

extern struct idx_desc *task_idx_desc_alloc(struct idx_desc_data *data);
extern int task_idx_desc_free(struct idx_desc *idx);
extern struct idx_desc_data *task_idx_data_alloc(const struct task_idx_ops *res_ops, void *fd_struct);
extern int task_idx_data_free(struct idx_desc *idx);

extern int task_self_idx_alloc(const struct task_idx_ops *ops, void *data);

static inline int task_self_idx_table_unbind(int fd) {
	return task_idx_table_unbind(task_self_idx_table(), fd);
}

static inline int task_self_idx_first_unbinded(void) {
	return task_idx_table_first_unbinded(task_self_idx_table());
}

static inline int task_valid_binded_fd(int fd) {
	return task_valid_fd(fd) && task_idx_table_is_binded(task_self_idx_table(), fd);
}

static inline int task_valid_unbinded_fd(int fd) {
	return task_valid_fd(fd) && !task_idx_table_is_binded(task_self_idx_table(), fd);
}

#endif /* TASK_IDX_H_ */

/**
 * @file
 * @brief
 *
 * @date 26.11.11
 * @author Anton Kozlov
 */

#ifndef TASK_RESOURCES_H_
#define TASK_RESOURCES_H_

#define CONFIG_TASKS_RES_QUANTITY 16

#include <stdarg.h>

enum {
	TASK_IDX_TYPE_FILE = 0,
	TASK_IDX_TYPE_SOCKET = 1
};

struct task_res_ops {
	int	type;
	int	(*open)(const char *path, int __oflag, va_list args);
	int	(*close)(int idx);
	int	(*read) (int fd, const void *buf, size_t nbyte);
	int	(*write)(int fd, const void *buf, size_t nbyte);
	int	(*ioctl)(int fd, int request, va_list args);
};

struct idx_desc {
	void *data;     /**< @brief Pointer for actual struct */
	int link_count; /**< @brief Count of links in all tasks */
	int type;	/**< @brief Type of resource */
};

static inline void *task_idx_desc_get_res(struct idx_desc *desc) {
	return desc->data;
}

static inline int task_idx_desc_get_type(struct idx_desc *desk) {
	return desk->type;
}

static inline int task_idx_desc_link_count(struct idx_desc *desc) {
	return desc->link_count;
}

static inline int task_idx_desc_link_count_add(struct idx_desc *desc, int d) {
	return (desc->link_count += d);
}

struct task_resources {
	// heap desc
	struct idx_desc *idx[CONFIG_TASKS_RES_QUANTITY];
};

#define TASK_IDX_RES(res, _idx) \
	(res->idx[_idx])

static inline struct idx_desc *task_res_idx_get(struct task_resources *res, int idx) {
	return TASK_IDX_RES(res, idx);
}

static inline void task_res_idx_set(struct task_resources *res, int idx, struct idx_desc *desc) {
	TASK_IDX_RES(res, idx) = desc;
}

#undef TASK_IDX_RES

extern struct idx_desc *task_idx_desc_alloc(int type, void *data);
extern void task_idx_desc_free(struct idx_desc *desc);

extern int task_res_idx_alloc(struct task_resources *res, int type, void *data);
extern void task_res_idx_free(struct task_resources *res, int idx);

static inline int task_res_idx_is_binded(struct task_resources *res, int idx) {
	return task_res_idx_get(res, idx) != NULL;
}

static inline void task_res_idx_unbind(struct task_resources *res, int idx) {
	task_res_idx_set(res, idx, NULL);
}

#endif


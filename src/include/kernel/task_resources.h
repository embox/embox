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

#include <assert.h>
#include <stdarg.h>

enum task_res_ops_type {
	TASK_RES_OPS_REGULAR,
	TASK_RES_OPS_TTY
};

/**
 * Specify operations with task's resources, which be called POSIX compat lib
 */
struct task_res_ops {
	int	(*read) (void *data, void *buf, size_t nbyte);
	int	(*write)(void *data, const void *buf, size_t nbyte);
	int	(*close)(void *data);
	int	(*ioctl)(void *data, int request, va_list args);
	int (*fseek)(void *data, long int offset, int origin);
	const enum task_res_ops_type type;
};

/**
 * Descriptor for numerated item for resource (int flides / int socket)
 * Task stores resources in this form, dynamically resolve operations
 * with resources by type
 */
struct idx_desc {
	const struct task_res_ops *res_ops;
	void *data;     /**< @brief Pointer for actual struct */
	int link_count; /**< @brief Count of links in all tasks */
};

/**
 * @brief idx utillity: get resource by idx
 * @param desc idx descriptor to get
 * @return resource in idx
 */
static inline void *task_idx_desc_data(struct idx_desc *desc) {
	assert(desc);
	return desc->data;
}

static inline const struct task_res_ops *task_idx_desc_ops(struct idx_desc *desc) {
	assert(desc);
	return desc->res_ops;
}

/**
 * @brief Allocate idx descriptor structure with type and data
 *
 * @param type idx descriptor type
 * @param data pointer for idx descriptor data
 *
 * @return
 */
extern struct idx_desc *task_idx_desc_alloc(const struct task_res_ops *res_ops, void *data);
extern void task_idx_desc_free(struct idx_desc *desc);

/**
 * @brief idx utillity: get reference count by idx
 * @param desc idx descriptor to get
 * @return count of references to idx resource
 */
static inline int task_idx_desc_link_count(struct idx_desc *desc) {
	assert(desc);
	return desc->link_count;
}

/**
 * @brief idx utillity: update reference count by idx
 * @param desc idx descriptor to update
 * @param d references count delta
 * @return new count of references to idx resource
 */
static inline int task_idx_desc_link_count_add(struct idx_desc *desc, int d) {
	assert(desc);
	return (desc->link_count += d);
}

/**
 * @brief Task resources container
 */
struct task_resources {
	struct idx_desc *idx[CONFIG_TASKS_RES_QUANTITY];
};

/**
 * @brief Get idx descriptor from task resources by idx number
 *
 * @param res Task resources to get idx from
 * @param idx idx number
 *
 * @return Pointer to idx
 */
static inline struct idx_desc *task_res_idx_get(struct task_resources *res, int idx) {
	assert(res);
	return res->idx[idx];
}

/**
 * @brief Set task resource's 'number' resource to idx
 *
 * @param res Task resource
 * @param idx number to set
 * @param desc idx descriptor to store in task resources
 */
void task_res_idx_set(struct task_resources *res, int idx, struct idx_desc *desc);

extern int task_res_idx_first_unbinded(struct task_resources *res);

/**
 * @brief Check if task resource number is occupied
 *
 * @param res Task resource to check
 * @param idx Number to check
 *
 * @return
 */
static inline int task_res_idx_is_binded(struct task_resources *res, int idx) {
	return task_res_idx_get(res, idx) != NULL;
}

/**
 * @brief Mark task resource's number as free. Use with care, old resource should
 * be properly freed
 *
 * @param res Task resource to deal with
 * @param idx idx number
 */
static inline void task_res_idx_unbind(struct task_resources *res, int idx) {
	task_res_idx_set(res, idx, NULL);
}

#endif


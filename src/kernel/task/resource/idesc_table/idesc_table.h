/**
 * @file
 * @brief
 *
 * @author Anton Bondarev
 * @author Ilia Vaprol
 * @date 25.02.14
 */

#ifndef KERNEL_TASK_RESOURCE_IDESC_TABLE_H_
#define KERNEL_TASK_RESOURCE_IDESC_TABLE_H_

#include <sys/cdefs.h>
#include <stdint.h>

#include <config/embox/kernel/task/resource/idesc_table.h>
#include <framework/mod/options.h>
#include <kernel/task.h>
#include <lib/libds/indexator.h>

#define MODOPS_IDESC_TABLE_SIZE \
	OPTION_MODULE_GET(embox__kernel__task__resource__idesc_table, \
			NUMBER, idesc_table_size)
struct idesc;

struct idesc_table {
	struct idesc *idesc_table[MODOPS_IDESC_TABLE_SIZE];

	struct indexator indexator;
	index_data_t index_buffer[INDEX_DATA_LEN(MODOPS_IDESC_TABLE_SIZE)];
};



#define idesc_cloexec_set(desc) \
	(desc = (struct idesc *)(((uintptr_t)desc) | 0x1))

#define idesc_cloexec_clear(desc) \
	(desc = (struct idesc *)(((uintptr_t)desc) & ~0x1))

#define idesc_is_cloexeced(idesc) ((uintptr_t)idesc & 0x1)

__BEGIN_DECLS

extern int idesc_index_valid(int idx);

extern int idesc_table_add(struct idesc_table *t, struct idesc *idesc,
		int cloexec);

extern void idesc_table_del(struct idesc_table *t, int idx);

extern int idesc_table_lock(struct idesc_table *t, struct idesc *idesc, int idx,
		int cloexec);

extern int idesc_table_locked(struct idesc_table *t, int idx);

extern void idesc_table_init(struct idesc_table *t);

extern void idesc_table_finit(struct idesc_table *t);

extern int idesc_table_fork(struct idesc_table *t, struct idesc_table *par_tab);

extern struct idesc *idesc_table_get(struct idesc_table *t, int idx);

extern struct idesc_table * task_resource_idesc_table(
		const struct task *task);

#define task_self_resource_idesc_table() \
	task_resource_idesc_table(task_self())

__END_DECLS

#endif /* KERNEL_TASK_RESOURCE_IDESC_TABLE_H_ */

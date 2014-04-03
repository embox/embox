/**
 * @file
 *
 * @date Nov 11, 2013
 * @author: Anton Bondarev
 */

#ifndef KERNEL_TASK_IDESC_TABLE_H_
#define KERNEL_TASK_IDESC_TABLE_H_

#include <kernel/task/resource/idesc_table.h>
#include <stdint.h>
#include <sys/cdefs.h>

struct idesc;
struct idesc_table;

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

__END_DECLS

#endif /* KERNEL_TASK_IDESC_TABLE_H_ */

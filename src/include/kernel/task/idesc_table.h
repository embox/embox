/**
 * @file
 *
 * @date Nov 11, 2013
 * @author: Anton Bondarev
 */

#ifndef IDESC_TABLE_H_
#define IDESC_TABLE_H_

//TODO idesc
#define IDESC_TABLE_USE

#include <framework/mod/options.h>
#include <module/embox/kernel/task/idx_table.h>

#define IDESC_QUANTITY \
	OPTION_MODULE_GET(embox__kernel__task__idx_table,NUMBER,task_res_quantity)

#define idesc_index_valid(idx) ((idx >=0) && (idx < IDESC_QUANTITY))

#include <util/indexator.h>

struct idesc;

struct idesc_table {
	struct idesc *idesc_table[IDESC_QUANTITY];

	struct indexator indexator;
	index_data_t index_buffer[INDEX_DATA_LEN(IDESC_QUANTITY)];
};

typedef struct idesc_table idesc_table_t;

#include <sys/cdefs.h>

__BEGIN_DECLS

extern int idesc_table_add(struct idesc_table *t, struct idesc *idesc,
		int cloexec);

extern int idesc_table_lock(struct idesc_table *t, struct idesc *idesc, int idx,
		int cloexec);

extern int idesc_table_locked(struct idesc_table *t, int idx);

extern int idesc_table_del(struct idesc_table *t, int idx);

extern struct idesc_table *idesc_table_get_table(struct task *task);

extern struct idesc *idesc_table_get_desc(struct idesc_table *t, int idx);

extern int idesc_table_init(struct idesc_table *t);

extern int idesc_table_finit(struct idesc_table *t);

extern int idesc_table_fork(struct idesc_table *t, struct idesc_table *par_tab);

extern struct idesc *idesc_common_get(int idx);

extern int idesc_is_cloexec(int fd);

extern int idesc_set_cloexec(int fd, int cloexec);


__END_DECLS

#endif /* IDESC_TABLE_H_ */

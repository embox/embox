/**
 * @file
 *
 * @date Nov 11, 2013
 * @author: Anton Bondarev
 */

#ifndef IDESC_TABLE_H_
#define IDESC_TABLE_H_


#include <framework/mod/options.h>
#include <module/embox/kernel/task/idx_table.h>

#define IDESC_QUANTITY \
	OPTION_MODULE_GET(embox__kernel__task__idx_table,NUMBER,task_res_quantity)

#include <util/indexator.h>

struct idesc;

struct idesc_table {
	struct idesc *idesc_table[IDESC_QUANTITY];
	struct indexator indexator;
};

typedef struct idesc_table idesc_table_t;


extern int idesc_table_add(struct idesc_table *t, struct idesc *idesc,
		int cloexec);

extern int idesc_table_lock(struct idesc_table *t, struct idesc *idesc, int idx,
		int cloexec);

extern int idesc_table_locked(struct idesc_table *t, int idx);

extern int idesc_table_del(struct idesc_table *t, int idx);

extern struct idesc *idesc_table_get(struct idesc_table *t, int idx);

extern int idesc_table_fork(struct idesc_table *t, struct idesc_table *par_tab);


#endif /* IDESC_TABLE_H_ */

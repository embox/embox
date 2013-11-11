/**
 * @file
 *
 * @date Nov 11, 2013
 * @author: Anton Bondarev
 */

#ifndef IDESC_TABLE_H_
#define IDESC_TABLE_H_


#include <kernel/task/idx.h>

struct idesc;

struct idesc_table {
	struct idesc *idesc_table[TASKS_RES_QUANTITY];
	struct indexator indexator;
};

typedef struct idesc_table idesc_table_t;


extern int idesc_table_add(struct idesc_table *t, struct idesc *idesc, int cloexec);
extern int idesc_table_lock(struct idesc_table *t, struct idesc *idesc, int idx, int cloexec);
extern int idesc_table_locked(struct idesc_table *t, int idx);
extern int idesc_table_del(struct idesc_table *t, int idx);
extern struct idesc *idesc_table_get(struct idesc_table *t, int idx);
extern int idesc_table_init(struct idesc_table *t, struct idesc_table *parent_table);


#endif /* IDESC_TABLE_H_ */

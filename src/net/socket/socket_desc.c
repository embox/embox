/**
 * @file
 *
 * @date Nov 1, 2013
 * @author: Anton Bondarev
 */
#include <stddef.h>

#include <kernel/task.h>
#include <kernel/task/idesc_table.h>
#include <kernel/task/resource/idesc_table.h>
#include <net/socket/socket_desc.h>
#include <fs/idesc.h>

extern const struct idesc_ops task_idx_ops_socket;

struct sock *idesc_sock_get(int idx) {
	struct idesc *idesc;
	struct idesc_table *it;


	assert(idesc_index_valid(idx));

	it = task_resource_idesc_table(task_self());
	assert(it);

	idesc = idesc_table_get(it, idx);
	if (idesc->idesc_ops != &task_idx_ops_socket) {
		return NULL;
	}

	return (struct sock *) idesc;
}

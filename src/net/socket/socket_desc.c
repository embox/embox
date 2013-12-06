/**
 * @file
 *
 * @date Nov 1, 2013
 * @author: Anton Bondarev
 */
#include <stddef.h>
#include <errno.h>

#include <kernel/task.h>
#include <kernel/task/idesc_table.h>
#include <net/socket/socket_desc.h>
#include <fs/idesc.h>

extern const struct idesc_ops task_idx_ops_socket;

int idesc_sock_get(int idx, struct sock **sk) {
	struct idesc *idesc;

	if (!idesc_index_valid(idx))
		return -EBADF;

	idesc = task_self_get_idesc(idx);

	if (!idesc)
		return -EBADF;

	if (idesc->idesc_ops != &task_idx_ops_socket)
		return -ENOTSOCK;

	*sk = (struct sock *) idesc;

	return 0;
}

/**
 * @file
 *
 * @date Nov 1, 2013
 * @author: Anton Bondarev
 */
#include <stddef.h>

#include <kernel/task.h>
#include <kernel/task/idesc_table.h>
#include <net/socket/socket_desc.h>

static struct socket_desc *socket_desc_alloc(void) {
	return NULL;
}

struct socket_desc *socket_desc_create(int domain, int type, int protocol) {
	return socket_desc_alloc();
}

void socket_desc_destroy(struct socket_desc *desc) {

}

void socket_desc_check_perm(struct socket_desc *desc, int ops_type,
		struct socket_desc_param *param) {

}

struct socket_desc *socket_desc_get(int idx) {
	struct idesc *idesc;
	struct idesc_table *it;


	assert(idesc_index_valid(idx));

	it = idesc_table_get_table(task_self());
	assert(it);

	idesc = idesc_table_get_desc(it, idx);

	return (struct socket_desc *) idesc;
}

struct socket_desc *socket_desc_accept(struct sock *sk) {
	return socket_desc_alloc();
}


struct sock *idesc_sock_get(int idx) {
	struct idesc *idesc;
	struct idesc_table *it;


	assert(idesc_index_valid(idx));

	it = idesc_table_get_table(task_self());
	assert(it);

	idesc = idesc_table_get_desc(it, idx);

	return (struct sock *) idesc;
}

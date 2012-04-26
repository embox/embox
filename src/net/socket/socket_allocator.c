/**
 * @file
 *
 * @brief
 *
 * @date 05.11.2011
 * @author Anton Bondarev
 */
#include <types.h>
#include <mem/misc/pool.h>
#include <net/net.h>
#include <net/socket.h>
#include <hal/ipl.h>

/* pool for allocate sockets */
POOL_DEF(socket_pool, struct socket, CONFIG_MAX_KERNEL_SOCKETS);

struct socket * socket_alloc(void) {
	struct socket *sock;
	ipl_t flags;

	flags = ipl_save();

	sock = pool_alloc(&socket_pool);

	ipl_restore(flags);

	return sock;
}

void socket_free(struct socket *sock) {
	ipl_t flags;

	flags = ipl_save();

	pool_free(&socket_pool, sock); /* return sock into pool */

	ipl_restore(flags);
}

/**
 * @file
 * @brief
 *
 * @date 05.11.11
 * @author Anton Bondarev
 */

#include <mem/misc/pool.h>
#include <net/socket.h>
#include <hal/ipl.h>

#include <framework/mod/options.h>

#define MODOPS_AMOUNT_SOCKET OPTION_GET(NUMBER, amount_socket)

/* pool for allocate sockets */
POOL_DEF(socket_pool, struct socket, MODOPS_AMOUNT_SOCKET);

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

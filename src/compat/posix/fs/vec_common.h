/**
 * @file
 * @brief
 *
 * @date 28.04.2016
 * @author Denis Chusovitin
 */

#include <unistd.h>
#include <errno.h>

#include <fs/index_descriptor.h>
#include <fs/idesc.h>
#include <kernel/task/resource/idesc_table.h>

#include <netinet/in.h>
#include <sys/socket.h>

#include <util/macro.h>
#include <kernel/task/resource/idesc_table.h>
#include <net/sock.h>
#include <net/socket/socket_desc.h>
#include <net/socket/ksocket.h>

#define  socket_idesc_check(sockfd, sk) {  \
	if (!idesc_index_valid(sockfd) || !index_descriptor_get(sockfd)) { \
		return SET_ERRNO(EBADF);            \
	}                                \
	if (NULL == (sk = idesc_sock_get(sockfd))) { \
		return SET_ERRNO(ENOTSOCK);            \
	} \
}

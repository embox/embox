/**
 * @file
 *
 * @date Nov 1, 2013
 * @author: Anton Bondarev
 */

#ifndef SOCKET_DESC_H_
#define SOCKET_DESC_H_

#include <net/sock.h>

extern int idesc_sock_get(int idx, struct sock **sk);


#endif /* SOCKET_DESC_H_ */

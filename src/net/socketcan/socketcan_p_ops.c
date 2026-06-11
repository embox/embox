/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 11.06.26
 */

#include <linux/can.h>

#include <lib/libds/dlist.h>
#include <net/net_sock.h>
#include <net/sock.h>

static DLIST_DEFINE(can_sock_list);

static const struct sock_proto_ops af_can_sock_ops_struct = {
    .sock_list = &can_sock_list,
};

EMBOX_NET_SOCK(AF_CAN, SOCK_RAW, CAN_RAW, 1, af_can_sock_ops_struct);

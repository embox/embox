/**
 * @file 
 * @brief Xen PV net driver initialization
 *
 * @date 
 * @author 
 */

#ifndef XEN_NET_H_
#define XEN_NET_H_

#define HOST_NET_IRQ 29
#define SCRATCH_LEN 1514

struct host_net_adp {
	int fd;
};

enum host_net_op {
	HOST_NET_INIT,
	HOST_NET_START,
	HOST_NET_STOP,
};

char host_net_scratch_rx[SCRATCH_LEN];

#endif /* XEN_NET_H_ */

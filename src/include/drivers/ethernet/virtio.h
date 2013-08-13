/**
 * @file
 * @brief
 *
 * @date 13.08.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_ETHERNET_VIRTIO_H_
#define DRIVERS_ETHERNET_VIRTIO_H_

#define VIRTIO_NET_F_CSUM       0       /* Host handles pkts w/ partial csum */
#define VIRTIO_NET_F_GUEST_CSUM 1       /* Guest handles pkts w/ partial csum */
#define VIRTIO_NET_F_MAC        5       /* Host has given MAC address. */
#define VIRTIO_NET_F_GSO        6       /* Host handles pkts w/ any GSO type */
#define VIRTIO_NET_F_GUEST_TSO4 7       /* Guest can handle TSOv4 in. */
#define VIRTIO_NET_F_GUEST_TSO6 8       /* Guest can handle TSOv6 in. */
#define VIRTIO_NET_F_GUEST_ECN  9       /* Guest can handle TSO[6] w/ ECN in. */
#define VIRTIO_NET_F_GUEST_UFO  10      /* Guest can handle UFO in. */
#define VIRTIO_NET_F_HOST_TSO4  11      /* Host can handle TSOv4 in. */
#define VIRTIO_NET_F_HOST_TSO6  12      /* Host can handle TSOv6 in. */
#define VIRTIO_NET_F_HOST_ECN   13      /* Host can handle TSO[6] w/ ECN in. */
#define VIRTIO_NET_F_HOST_UFO   14      /* Host can handle UFO in. */
#define VIRTIO_NET_F_MRG_RXBUF  15      /* Host can merge receive buffers. */
#define VIRTIO_NET_F_STATUS     16      /* virtio_net_config.status available */
#define VIRTIO_NET_F_CTRL_VQ    17      /* Control channel available */
#define VIRTIO_NET_F_CTRL_RX    18      /* Control channel RX mode support */
#define VIRTIO_NET_F_CTRL_VLAN  19      /* Control channel VLAN filtering */
#define VIRTIO_NET_F_CTRL_RX_EXTRA 20   /* Extra RX mode control support */
#define VIRTIO_NET_F_GUEST_ANNOUNCE 21  /* Guest can announce device on the network */
#define VIRTIO_NET_F_MQ 22      /* Device supports Receive Flow
                                 * Steering */
#define VIRTIO_NET_F_CTRL_MAC_ADDR 23   /* Set MAC address */

#define VIRTIO_NET_S_LINK_UP    1       /* Link is up */
#define VIRTIO_NET_S_ANNOUNCE   2       /* Announcement is needed */

#endif /* DRIVERS_ETHERNET_VIRTIO_H_ */

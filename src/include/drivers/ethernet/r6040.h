/**
 * @file
 * @brief r6040 ethernet driver
 *
 * @date 03.07.11
 * @author Nikolay Korotkiy
 */

#ifndef NET_R6040_H_
#define NET_R6040_H_

#include <net/netdevice.h>

#define R6040_RX_DESCRIPTORS 32
#if 0
/* setup descriptors, start packet reception */
extern void r6040_init(void);
#endif

extern void r6040_rx_enable(void);

extern int r6040_open(struct net_device *dev);

#if 0
/* Disable packet reception */
extern void r6040_done(void);
#endif

/* queue packet for transmission */
extern void r6040_tx(unsigned char* pkt, size_t length);

/* Returns size of pkt, or zero if none received */
extern size_t r6040_rx(unsigned char* pkt, size_t max_len);

extern unsigned short r6040_mdio_read(int, int);

extern int r6040_wait_linkup(void);

#endif /* NET_R6040_H_ */

/**
 * @file
 * @brief r6040 ethernet driver
 *
 * @date 03.07.11
 * @author Nikolay Korotkiy
 */

#ifndef R6040_H_
#define R6040_H_

#define R6040_RX_DESCRIPTORS 32

/* setup descriptors, start packet reception */
extern void r6040_init(void);

extern void r6040_rx_enable(void);

/* Disable packet reception */
extern void r6040_done(void);

/* queue packet for transmission */
extern void r6040_tx(unsigned char* pkt, size_t length);

/* Returns size of pkt, or zero if none received */
extern size_t r6040_rx(unsigned char* pkt, size_t max_len);

extern unsigned short r6040_mdio_read(int, int);

extern int r6040_wait_linkup(void);

#endif /* R6040_H_ */

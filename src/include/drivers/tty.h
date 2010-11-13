/**
 * @file
 *
 * @brief Describes tty devices interface and structures
 *
 * @date 13.11.2010
 * @author Anton Bondarev
 */

#ifndef TTY_H_
#define TTY_H_

#include <types.h>

#define TTY_RXBUFF_SIZE 0x80
#define TTY_TXBUFF_SIZE 0x80

typedef struct tty_device {
	uint8_t  out_buff[TTY_RXBUFF_SIZE + 1];
	uint8_t  rx_buff[TTY_RXBUFF_SIZE + 1];
	uint8_t  tx_buff[TTY_TXBUFF_SIZE + 1];
	uint32_t rx_cnt;
	bool     out_busy; /*whether out_buff is busy*/
} tty_device_t;


extern int tty_register(void) ;

extern int tty_get_uniq_number(void);

/*
 * add parsed char to receive buffer
 */
extern int tty_add_char(tty_device_t *tty, int ch);

extern uint8_t* tty_readline(tty_device_t *tty);

extern void tty_freeline(tty_device_t *tty, uint8_t *line);

#endif /* TTY_H_ */

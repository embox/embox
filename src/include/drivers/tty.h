/**
 * @file
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

#include <fs/file.h>
#include <drivers/vtbuild.h>
#include <drivers/vtparse.h>
#include <drivers/vconsole.h>

struct vconsole;

typedef volatile struct tty_device {
	uint8_t  out_buff[TTY_RXBUFF_SIZE + 1];
	uint32_t rx_cur;
	uint8_t  rx_buff[TTY_RXBUFF_SIZE + 1];
	uint8_t  tx_buff[TTY_TXBUFF_SIZE + 1];
	uint32_t rx_cnt;
	bool     out_busy; /*whether out_buff is busy*/
	bool     ins_mod;
	#if 1 /* to-do: USE IT! */
	struct 	 vtbuild vtb[1];
	struct 	 vtparse vtp[1];
	bool	 has_init;
	#endif
	#ifdef CONFIG_TTY_CONSOLE_COUNT
	struct vconsole console[CONFIG_TTY_CONSOLE_COUNT];
	//struct vconsole_t console[CONFIG_TTY_CONSOLE_COUNT];
	//struct vconsole *console;//[CONFIG_TTY_CONSOLE_COUNT];
	uint32_t console_cur;
	#endif
	file_operations_t *file_op;
} tty_device_t;

extern tty_device_t *cur_tty;

extern int tty_register(tty_device_t *tty);

extern int tty_unregister(tty_device_t *tty);

extern int tty_get_uniq_number(void);

/**
 * add parsed char to receive buffer
 */
extern int tty_add_char(tty_device_t *tty, int ch);

extern uint8_t* tty_readline(tty_device_t *tty);

extern void tty_freeline(tty_device_t *tty, uint8_t *line);

#endif /* TTY_H_ */

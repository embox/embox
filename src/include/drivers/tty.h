/**
 * @file
 * @brief Describes tty devices interface and structures
 *
 * @date 13.11.10
 * @author Anton Bondarev
 */

#ifndef TTY_H_
#define TTY_H_

#include <types.h>

#ifndef TTY_RXBUFF_SIZE
#define TTY_RXBUFF_SIZE 32
#endif
#ifndef TTY_TXBUFF_SIZE
#define TTY_TXBUFF_SIZE 0x1000
#endif

#include <fs/file.h>
#include <drivers/vtbuild.h>
#include <drivers/vtparse.h>
#include <drivers/vconsole.h>

struct vconsole;

#define ENABLE_CONSOLE_QUEUE 0 /* don't work TODO debug me!!! */

#define CONFIG_TTY_QUEUE 32

#if 0
#ifndef CONFIG_TTY_CONSOLE_COUNT
#define	CONFIG_TTY_CONSOLE_COUNT 1
#endif
#endif

#define TTY_RX_QUEUE_INIT(TTY) do { TTY->rx_beg = 0; TTY->rx_end = 0; } while(0)
#define TTY_RX_QUEUE_FULL(TTY) ( ( TTY_RXBUFF_SIZE + 1 + TTY->rx_end - TTY->rx_beg ) % (TTY_RXBUFF_SIZE + 1) \
	== (TTY_RXBUFF_SIZE))
#define TTY_RX_QUEUE_PUSH(TTY,ELEMENT) \
	do { \
		if (TTY_RX_QUEUE_FULL(TTY)) break; \
		TTY->rx_buff[TTY->rx_end++] = ELEMENT; \
		TTY->rx_end %= TTY_RXBUFF_SIZE + 1; \
		TTY->has_events = true; \
	} while (0)
#define TTY_RX_QUEUE_EMPTY(TTY) ( TTY->rx_beg == TTY->rx_end )
#define TTY_RX_QUEUE_POP(TTY,ELEMENT_LVALUE) \
	do { \
		ELEMENT_LVALUE = TTY->rx_buff[TTY->rx_beg++]; \
		TTY->rx_beg %= TTY_RXBUFF_SIZE + 1; \
	} while (0)

typedef struct tty_device {
	volatile bool out_busy; /*TODO move to vconsole */
	volatile char out_buff[TTY_RXBUFF_SIZE + 1];
	volatile bool has_events;
	volatile bool handle_events;
	volatile uint32_t rx_cur;
	char  rx_buff[TTY_RXBUFF_SIZE + 1];
	char  tx_buff[TTY_TXBUFF_SIZE + 1];
	volatile uint32_t rx_cnt;
	uint32_t rx_beg;
	uint32_t rx_end;
	volatile bool     ins_mod;
	struct 	 vtbuild vtb[1];
	struct 	 vtparse vtp[1];
	#ifdef CONFIG_TTY_CONSOLE_COUNT
	struct vconsole *consoles[CONFIG_TTY_CONSOLE_COUNT];
	#endif
	uint32_t console_cur;
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

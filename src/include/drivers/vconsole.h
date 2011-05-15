/**
 * @file
 * @brief Defines virtual console interface.
 * A virtual console is an instance of the tty device.
 *
 * @date 23.11.10
 * @author Anton Bondarev
 */

#ifndef VCONSOLE_H_
#define VCONSOLE_H_

#include <stdint.h>
#include <stddef.h>

#ifndef CONFIG_SCR_BUF_S
#define CONFIG_SCR_BUFF_S (80 * 25)
#endif

#ifndef CONFIG_CMDLINE_S
#define CONFIG_CMDLINE_S 80
#endif

#ifndef CONFIG_ESH_STACK_S
#define CONFIG_ESH_STACK_S (4096 * 10)
#endif

struct tty_device;

#ifndef TTY_RXBUFF_SIZE
#define TTY_RXBUFF_SIZE 32
#endif
#ifndef TTY_TXBUFF_SIZE
#define TTY_TXBUFF_SIZE 0x1000
#endif

#define CONS_TX_QUEUE_INIT(CONS) do { CONS->tx_beg = 0; CONS->tx_end = 0; } while(0)
#define CONS_TX_QUEUE_FULL(CONS) ( ( TTY_TXBUFF_SIZE + CONS->tx_end - CONS->tx_beg ) % (TTY_TXBUFF_SIZE) \
	== (TTY_TXBUFF_SIZE-1))
#define CONS_TX_QUEUE_PUSH(CONS,ELEMENT) \
	do { \
		if ( CONS_TX_QUEUE_FULL(CONS) ) break; \
		CONS->tx_buff[CONS->tx_end++] = ELEMENT; \
		CONS->tx_end %= TTY_TXBUFF_SIZE; \
		CONS->tty->has_events = true; \
	} while (0)
#define CONS_TX_QUEUE_EMPTY(CONS) ( CONS->tx_beg == CONS->tx_end )
#define CONS_TX_QUEUE_POP(CONS,ELEMENT_LVALUE) \
	do { \
		ELEMENT_LVALUE = CONS->tx_buff[CONS->tx_beg++]; \
		CONS->tx_beg %= TTY_TXBUFF_SIZE; \
	} while (0)

typedef struct vconsole {
	uint8_t  id;
	struct	 tty_device *tty;
	size_t   height;
	size_t   width;
	uint32_t mode;

	uint8_t  tx_buff[TTY_TXBUFF_SIZE + 1];
	uint32_t tx_beg;
	uint32_t tx_end;
	uint8_t  scr_buff[CONFIG_SCR_BUFF_S];
	uint8_t  scr_line;		/* current position of cursor */
	uint8_t  scr_column;

	uint32_t cl_cnt;
	uint32_t cl_cur;
	uint8_t  cl_buff[CONFIG_CMDLINE_S]; /* current terminal string (cannonical mode) */

	volatile bool out_busy; /* tty_readline output */
	uint8_t  out_buff[CONFIG_CMDLINE_S];
} vconsole_t;

struct tty_device;
extern vconsole_t *vconsole_create(int id, struct tty_device *tty) ;


extern vconsole_t const *sys_console;
extern vconsole_t *cur_console;

extern int vconsole_activate(vconsole_t *con);
extern int vconsole_deactivate(vconsole_t *con);

extern void vconsole_putcharXY(struct vconsole *vc, uint8_t x, uint8_t y, char ch);
extern char vconsole_getchar(struct vconsole *vc);
extern void vconsole_gotoxy(struct vconsole *vc, uint8_t x, uint8_t y);
extern void vconsole_clear(struct vconsole *vc);
/*
extern void vconsole_reprint(struct vconsole *vc);
extern void vconsole_reprint_nline(struct vconsole *vc, uint8_t h);
*/

#include <drivers/tty.h>

#endif /* VCONSOLE_H_ */

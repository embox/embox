/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.09.2013
 */

#ifndef DRIVERS_TERMIOS_OPS_H_

#include <stddef.h>
#include <termios.h>

#include <lib/libds/ring.h>

#define DRIVERS_TERMIOS_OPS_H_

#define TERMIOS_CC_INIT                                                \
	{                                                                  \
		[VEOF] = __TERMIOS_CTRL('d'), [VEOL] = ((cc_t)~0), /* undef */ \
		    [VERASE] = 0177, [VINTR] = __TERMIOS_CTRL('c'),            \
		[VKILL] = __TERMIOS_CTRL('u'), [VMIN] = 1,                     \
		[VQUIT] = __TERMIOS_CTRL('\\'), [VTIME] = 0,                   \
		[VSUSP] = __TERMIOS_CTRL('z'), [VSTART] = __TERMIOS_CTRL('q'), \
		[VSTOP] = __TERMIOS_CTRL('s'),                                 \
	}

#define __TERMIOS_CTRL(ch) (cc_t)((ch)&0x1f)

#define TERMIOS_IFLAG_INIT (tcflag_t)(BRKINT | ICRNL | IXON)
#define TERMIOS_OFLAG_INIT (tcflag_t)(OPOST | ONLCR | OXTABS)
#define TERMIOS_CFLAG_INIT (tcflag_t)(CREAD | CS8 | HUPCL)
#define TERMIOS_LFLAG_INIT \
	(tcflag_t)(ICANON | ISIG | ECHO | ECHOE | ECHOK | ECHONL)

#define TERMIOS_RES_GOT_DATA    (1 << 0)
#define TERMIOS_RES_GOT_ECHO    (1 << 1)
#define TERMIOS_RES_GOT_NEWLINE (1 << 2)

#define TERMIOS_TIME_INF        ((unsigned long)-1)

#define TIO_I(t, flag)          ((t)->c_iflag & (flag))
#define TIO_O(t, flag)          ((t)->c_oflag & (flag))
#define TIO_C(t, flag)          ((t)->c_cflag & (flag))
#define TIO_L(t, flag)          ((t)->c_lflag & (flag))

#define TTY_I(t, flag)          ((t)->termios.c_iflag & (flag))
#define TTY_O(t, flag)          ((t)->termios.c_oflag & (flag))
#define TTY_C(t, flag)          ((t)->termios.c_cflag & (flag))
#define TTY_L(t, flag)          ((t)->termios.c_lflag & (flag))

typedef struct termios_i_buff {
	struct ring *ring;
	char *buff;
	struct ring *canon_ring;
	size_t buflen;
} termios_i_buff;

/**
 * @brief Does associated with termios mapping of input symbol to string,
 * i.e. \r -> \r\n
 *
 * @return Number of writed symbols
 */
extern int termios_putc(const struct termios2 *t, char ch, struct ring *ring,
    char *buf, size_t buflen);

/**
 * @brief Does associated with termios mapping of symbol to it's visual
 * representation string.
 *
 * @return
 */
extern int termios_gotc(const struct termios2 *t, char ch, struct ring *ring,
    char *buf, size_t buflen);

/**
 * @brief Processes the current character after reading.
 *
 * @return
 */
extern int termios_input(const struct termios2 *t, char ch,
    struct termios_i_buff *b, struct ring *o_ring, char *o_buff, size_t buflen);

/**
 * @brief
 *
 * @return Pointer to the next characters for reading.
 */
extern char *termios_read(const struct termios2 *t, struct termios_i_buff *b,
    char *buff, char *end);

/**
 * @brief Sets the buffer size and timeout.
 */
extern void termios_update_size(const struct termios2 *t, size_t *size,
    unsigned long *timeout);

/**
 * @brief Updates rings depending on the current state of termios.
 */
extern void termios_update_ring(const struct termios2 *t, struct ring *ring,
    struct ring *canon_ring);

/**
 * @brief Checks whether termios can read.
 *
 * @return
 */
extern int termios_can_read(const struct termios2 *t, struct ring *ring,
    struct ring *canon_ring, size_t buflen);

/**
 * @brief Initializes termios.
 */
extern void termios_init(struct termios2 *t);

/**
 * @brief Initializes input buffer.
 */
extern void termios_i_buff_init(struct termios_i_buff *b, struct ring *ring,
    char *buff, struct ring *canon_ring, size_t buflen);

#endif /* DRIVERS_TERMIOS_OPS_H_ */

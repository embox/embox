/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.09.2013
 */

#ifndef DRIVERS_TERMIOS_OPS_H_

#include <util/ring.h>

#define DRIVERS_TERMIOS_OPS_H_

#define TERMIOS_CC_INIT \
	{ \
		[VEOF]   = __TERMIOS_CTRL('d'),  \
		[VEOL]   = ((cc_t) ~0), /* undef */ \
		[VERASE] = 0177,             \
		[VINTR]  = __TERMIOS_CTRL('c'),  \
		[VKILL]  = __TERMIOS_CTRL('u'),  \
		[VMIN]   = 1,                \
		[VQUIT]  = __TERMIOS_CTRL('\\'), \
		[VTIME]  = 0,                \
		[VSUSP]  = __TERMIOS_CTRL('z'),  \
		[VSTART] = __TERMIOS_CTRL('q'),  \
		[VSTOP]  = __TERMIOS_CTRL('s'),  \
	}

#define __TERMIOS_CTRL(ch)  (cc_t) ((ch) & 0x1f)

#define TERMIOS_IFLAG_INIT  (tcflag_t) (BRKINT | ICRNL | IXON)
#define TERMIOS_OFLAG_INIT  (tcflag_t) (OPOST | ONLCR | OXTABS)
#define TERMIOS_CFLAG_INIT  (tcflag_t) (CREAD | CS8 | HUPCL)
#define TERMIOS_LFLAG_INIT  (tcflag_t) (ICANON | ISIG | \
			ECHO | ECHOE | ECHOK | ECHONL)

struct ring;
struct termios;

/**
 * @brief Does associated with termios mapping of input symbol to string,
 * i.e. \r -> \r\n
 *
 * @return Number of writed symbols
 */
extern int termios_putc(const struct termios *tio, char ch,
		struct ring *ring, char *buf, size_t buflen);

/**
 * @brief Does associated with termios mapping of symbol to it's visual
 * representation string.
 *
 * @return
 */
extern int termios_gotc(const struct termios *tio, char ch,
		struct ring *ring, char *buf, size_t buflen);

/**
 * @brief
 *
 * @return
 */
extern int termios_get_status(const struct termios *tio, char *verase);

/**
 * @brief Provides newline control: IGNCR, ICRNL, INLCR.
 *
 * @return Value that shows if newline was handled.
 */
extern int termios_handle_newline(const struct termios *tio,
		char ch, int *is_eol);

/**
 * @brief Provides erase/kill control: VKILL, VERASE.
 *
 * @return
 */
extern int termios_handle_erase(const struct termios *tio, char ch, int *raw_mode,
		struct ring *ring, struct ring *canon_ring, size_t buflen);

/**
 * @brief
 *
 * @return
 */
extern int termios_get_data(const struct termios *tio, char ch,
		struct ring *ring, struct ring *canon_ring);

/**
 * @brief
 *
 * @return
 */
extern char *termios_read(const struct termios *tio, char *buff, char *end,
		struct ring *ring, struct ring *canon_ring, char *i_buff, size_t buflen);

/**
 * @brief
 *
 * @return
 */
extern size_t termios_update_size(const struct termios *tio,
		size_t size, unsigned long *timeout);

/**
 * @brief
 */
extern void termios_update_ring(const struct termios *tio, 
		struct ring *ring, struct ring *canon_ring);

/**
 * @brief
 *
 * @return
 */
extern int termios_get_result(const struct termios *tio,
		struct ring *ring, struct ring *canon_ring, size_t buflen);

/**
 * @brief Initializes termios.
 */
extern void termios_init(struct termios *tio);

#endif /* DRIVERS_TERMIOS_OPS_H_ */

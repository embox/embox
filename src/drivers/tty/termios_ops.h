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
 * @brief Checks the current status of ECHO and ECHOE flags.
 *
 * @return Value that depends on which flag is enabled. 
 */
extern int termios_echo_status(const struct termios *tio, char *verase);

/**
 * @brief Provides newline control: IGNCR, ICRNL, INLCR.
 *
 * @return Value that indicates whether a newline was handled.
 */
extern int termios_handle_newline(const struct termios *tio,
		char ch, int *is_eol);

/**
 * @brief Provides erase/kill control: VKILL, VERASE.
 *
 * @return Number of characters to erase.
 */
extern int termios_handle_erase(const struct termios *tio, char ch, 
		int *raw_mode, struct ring *ring, struct ring *canon_ring, size_t buflen);

/**
 * @brief Updates the ring and checks status of a buffer after 
 * processing input character.
 *
 * @return
 */
extern int termios_input_status(const struct termios *tio, char ch,
		struct ring *ring, struct ring *canon_ring);

/**
 * @brief Processes the current character after reading.
 *
 * @return Pointer to the next character to read.
 */
extern char *termios_read(const struct termios *tio, char *buff, char *end,
		struct ring *ring, struct ring *canon_ring, char *i_buff, size_t buflen);

/**
 * @brief Sets the buffer size and timeout.
 */
extern void termios_update_size(const struct termios *tio,
		size_t *size, unsigned long *timeout);

/**
 * @brief Updates rings depending on the current state of termios.
 */
extern void termios_update_ring(const struct termios *tio, 
		struct ring *ring, struct ring *canon_ring);

/**
 * @brief Checks whether termios can read.
 *
 * @return
 */
extern int termios_can_read(const struct termios *tio,
		struct ring *ring, struct ring *canon_ring, size_t buflen);

/**
 * @brief Initializes termios.
 */
extern void termios_init(struct termios *tio);

#endif /* DRIVERS_TERMIOS_OPS_H_ */

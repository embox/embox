/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.09.2013
 */

#ifndef DRIVERS_TERMIOS_OPS_H_
#define DRIVERS_TERMIOS_OPS_H_

#define TTY_TERMIOS_CC_INIT \
    { \
        [VEOF]   = __TTY_CTRL('d'),  \
        [VEOL]   = ((cc_t) ~0), /* undef */ \
        [VERASE] = 0177,             \
        [VINTR]  = __TTY_CTRL('c'),  \
        [VKILL]  = __TTY_CTRL('u'),  \
        [VMIN]   = 1,                \
        [VQUIT]  = __TTY_CTRL('\\'), \
        [VTIME]  = 0,                \
        [VSUSP]  = __TTY_CTRL('z'),  \
        [VSTART] = __TTY_CTRL('q'),  \
        [VSTOP]  = __TTY_CTRL('s'),  \
    }

#define __TTY_CTRL(ch)  (cc_t) ((ch) & 0x1f)

#define TTY_TERMIOS_IFLAG_INIT  (tcflag_t) (BRKINT | ICRNL | IXON)
#define TTY_TERMIOS_OFLAG_INIT  (tcflag_t) (OPOST | ONLCR | OXTABS)
#define TTY_TERMIOS_CFLAG_INIT  (tcflag_t) (CREAD | CS8 | HUPCL)
#define TTY_TERMIOS_LFLAG_INIT  (tcflag_t) (ICANON | ISIG | \
            ECHO | ECHOE | ECHOK | ECHONL)

struct ring;
struct termios;

/**
 * @brief Does associated with termios mapping of input symbol to string,
 * i.e. \r -> \r\n
 *
 * @param tio
 * @param ch
 * @param ring
 * @param buf
 * @param buflen
 *
 * @return Number of writed symbols
 */
extern int termios_putc(const struct termios *tio, char ch,
        struct ring *ring, char *buf, size_t buflen);

/**
 * @brief Does associated with termios mapping of symbol to it's visual
 * representation string.
 *
 * @param tio
 * @param ch
 * @param ring
 * @param buf
 * @param buflen
 *
 * @return
 */
extern int termios_gotc(const struct termios *tio, char ch,
        struct ring *ring, char *buf, size_t buflen);

// TODO: Write comments for functions below.

/**
 * @brief 
 *
 * @param tio
 * @param flag
 *
 * @return
 */
extern int termios_get_lflag(const struct termios *tio, int flag);

/**
 * @brief 
 *
 * @param tio
 * @param flag
 *
 * @return
 */
extern int termios_get_cc(const struct termios *tio, int flag);

/**
 * @brief Newline control: IGNCR, ICRNL, INLCR
 *
 * @param tio
 * @param ch
 * @param is_eol
 *
 * @return
 */
extern int termios_handle_newline(const struct termios *tio, 
        char ch, int *is_eol);

/**
 * @brief
 *
 * @param tio
 * @param ch
 * @param erase_all
 *
 * @return
 */
extern int termios_handle_erase(const struct termios *tio, 
        char ch, int *erase_all);

/**
 * @brief
 *
 * @param tio
 * @param ch
 * @param is_eof
 *
 * @return
 */
extern int termios_check_end(const struct termios *tio, char ch, int *is_eof);

/**
 * @brief
 *
 * @param tio
 * @param size
 * @param timeout
 *
 * @return
 */
extern size_t termios_get_size(const struct termios *tio, 
        size_t size, unsigned long *timeout);

/**
 * @brief Init
 *
 * @param tio
 *
 * @return
 */
extern void termios_init(struct termios *tio);

#endif /* DRIVERS_TERMIOS_OPS_H_ */
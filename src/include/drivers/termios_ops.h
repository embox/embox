/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.09.2013
 */

#ifndef DRIVERS_TERMIOS_OPS_H_
#define DRIVERS_TERMIOS_OPS_H_

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
extern int termios_putc(struct termios *tio, char ch, struct ring *ring,
		char *buf, size_t buflen);

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
extern int termios_gotc(struct termios *tio, char ch, struct ring *ring,
		char *buf, size_t buflen);

#endif /* DRIVERS_TERMIOS_OPS_H_ */


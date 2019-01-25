/**
 * @file
 *
 * @date Dec 18, 2013
 * @author: Anton Bondarev
 */

#ifndef IDESC_SERIAL_H_
#define IDESC_SERIAL_H_

#include <sys/types.h>
struct idesc;
struct uart;

extern struct idesc *idesc_serial_create(struct uart *uart, mode_t mod);

extern const struct idesc_ops *idesc_serial_get_ops(void);

extern void idesc_serial_close(struct idesc *idesc);

#endif /* IDESC_SERIAL_H_ */

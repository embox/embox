/**
 * @file
 *
 * @date Dec 18, 2013
 * @author: Anton Bondarev
 */

#ifndef IDESC_SERIAL_H_
#define IDESC_SERIAL_H_

struct idesc;
struct file_desc;
struct uart;
typedef int idesc_access_mode_t;

extern struct idesc *idesc_serial_create(struct file_desc *fdesc,
		struct uart *uart, idesc_access_mode_t mod) ;

#endif /* IDESC_SERIAL_H_ */

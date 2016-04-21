/**
 * @file
 *
 * @date 21.04.2016
 * @author Anton Bondarev
 */

#ifndef TTYS_H_
#define TTYS_H_

#include <drivers/char_dev.h>
#define TTYS_DEF(name, uart) \
		extern const struct file_operations ttys_fops; \
		CHAR_DEV_DEF(name, (struct file_operations *)&ttys_fops, NULL, uart)

#endif /* TTYS_H_ */

/**
 * @file
 * @brief char devices
 *
 * @date 07.07.10
 * @author Fedor Burdun
 */

#ifndef CHAR_DEVICE_H
#define CHAR_DEVICE_H

typedef struct chardev {
	int (*putc) (void);
	int (*getc) (int);
} chardev_t;
#if 0
#define MAX_COUNT_CHAR_DEVICES 10
static chardev_t pool_chardev[MAX_COUNT_CHAR_DEVICES];
static int chardev_c = 0;

#define ADD_CHAR_DEVICE(__NAME,__IN,__OUT) \
	int __NAME = 0;                    \
	__NAME = chardev_c;                \
	pool_chardev[chardev_c++] = {      \
		.getc = __IN;              \
		.putc = __OUT;             \
	};
#endif /* 0 */

#endif /* CHAR_DEVICE_H */

/**
 * @file
 * @brief
 *
 * @date 13.02.10
 * @author Eldar Abusalimov
 * @author Ilia Vaprol
 * @author Anton Kozlov
 */

#ifndef DRIVERS_DIAG_H_
#define DRIVERS_DIAG_H_

#include <util/macro.h>

struct diag_ops {
	int (*init)(void);
	char (*getc)(void);
	void (*putc)(char ch);
	int (*kbhit)(void);
};

extern int diag_init(void);
extern char diag_getc(void);
extern void diag_putc(char ch);
extern int diag_kbhit(void);

#define DIAG_OPS_NAME(_modname) \
	MACRO_CONCAT(_modname, _diag_ops)

#define DIAG_OPS_DECLARE(...) \
	const struct diag_ops DIAG_OPS_NAME(__EMBUILD_MOD__) = { __VA_ARGS__ }

#endif /* DRIVERS_DIAG_H_ */

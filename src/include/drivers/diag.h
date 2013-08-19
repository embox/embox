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

struct diag {
	int (*init)(const struct diag *diag);
	char (*getc)(const struct diag *diag);
	void (*putc)(const struct diag *diag, char ch);
	int (*kbhit)(const struct diag *diag);

	void *obj;
};

enum diag_kbhit_ret {
	KBHIT_CAN_GETC = 0, /**< Will not block, getc will return immediatly */
	KBHIT_WILL_BLK = 1, /**< Will block awaiting input */
	KBHIT_WILL_FOREVER, /**< Have no getc capability, getc will take forever */
};

/**
 * @brief Initialize diag input/output
 *
 * @return 0
 */
extern int diag_init(void);

/**
 * @brief Put char onto diag output.
 *
 * @param ch
 */
extern void diag_putc(char ch);

/**
 * @brief Get symbol. Can take unpredicatable time awaiting input.
 *
 * @return Valid input character
 */
extern char diag_getc(void);

/**
 * @brief Test diag input buffer.
 *
 * @return see @a diag_kbhit_ret description
 */
extern enum diag_kbhit_ret diag_kbhit(void);

#define DIAG_OPS_NAME(_modname) \
	MACRO_CONCAT(_modname, _diag_ops)

#define DIAG_OPS_DECLARE(...) \
	const struct diag DIAG_OPS_NAME(__EMBUILD_MOD__) = { __VA_ARGS__ }

#endif /* DRIVERS_DIAG_H_ */

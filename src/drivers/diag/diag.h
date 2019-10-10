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

struct diag;

struct diag_ops {
	int (*init)(const struct diag *diag);
	char (*getc)(const struct diag *diag);
	void (*putc)(const struct diag *diag, char ch);
	int (*kbhit)(const struct diag *diag);
};

struct diag {
	const struct diag_ops *ops;
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
 * @brief Get symbol. Can take unpredictable time awaiting input.
 *
 * @return Valid input character
 */
extern char diag_getc(void);

/**
 * @brief Test diag input buffer.
 *
 * @return 1 if there is a symbol for getc() 0 in other cases
 */
extern int diag_kbhit(void);

#define DIAG_IMPL_NAME(_modname) \
	MACRO_CONCAT(_modname, diag_impl)

#define DIAG_OPS_DEF(...) \
		static const struct diag_ops __DIAG_OPS_NAME(__EMBUILD_MOD__) = { __VA_ARGS__ }; \
		const struct diag DIAG_IMPL_NAME(__EMBUILD_MOD__) = { \
			.ops = &__DIAG_OPS_NAME(__EMBUILD_MOD__), \
		}

#define __DIAG_OPS_NAME(_modname) \
	MACRO_CONCAT(_modname, _diag_ops)

#endif /* DRIVERS_DIAG_H_ */

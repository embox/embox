/**
 * @file
 * @brief
 *
 * @author Denis Deryugin
 * @date 26 Jan 2015
 */

#ifndef DRIVERS_SDIO_H_
#define DRIVERS_SDIO_H_

#include <util/macro.h>

struct sdio;

struct sdio_ops {
	int (*init)(const struct diag *diag);
	char (*getc)(const struct diag *diag);
	void (*putc)(const struct diag *diag, char ch);
	int (*kbhit)(const struct diag *diag);
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

/**
 * @brief Set new diag interface. Intended for some emergency situation, when
 * new diag is known only in runtime
 *
 * @param diag
 *
 * @return 0 on success
 * @return Negative otherwise
 */
extern int diag_setup(const struct diag *diag);

#define DIAG_IMPL_NAME(_modname) \
	MACRO_CONCAT(_modname, diag_impl)


#define DIAG_OPS_DECLARE(...) \
	static const struct diag_ops __DIAG_OPS_NAME(__EMBUILD_MOD__) = { __VA_ARGS__ }; \
	const struct diag DIAG_IMPL_NAME(__EMBUILD_MOD__) = { \
		.ops = &__DIAG_OPS_NAME(__EMBUILD_MOD__), \
       	}

#define __DIAG_OPS_NAME(_modname) \
	MACRO_CONCAT(_modname, _diag_ops)

#endif /* DRIVERS_DIAG_H_ */

/**
 * @file
 * @brief Unit entity.
 * @details
 *   Unit is the simplest and most generic superstructure over the mod entity
 *   independent from any frameworks.
 *
 * @date 10.03.10
 * @author Eldar Abusalimov
 */

#ifndef EMBOX_UNIT_H_
#define EMBOX_UNIT_H_

#include <stddef.h>

#include <embox/kernel.h>
#include <framework/mod/self.h>

#define __EMBOX_UNIT(_init, _fini) \
	static const struct unit __unit = { \
		.init = _init, \
		.fini = _fini, \
	}; \
	MOD_INFO_BIND(&__unit_mod_ops, &__unit)

#define EMBOX_UNIT(_init, _fini) \
	static int _init(void); \
	static int _fini(void); \
	__EMBOX_UNIT(_init, _fini)

#define EMBOX_UNIT_INIT(_init) \
	static int _init(void); \
	__EMBOX_UNIT(_init, NULL)

#define EMBOX_UNIT_FINI(_fini) \
	static int _fini(void); \
	__EMBOX_UNIT(NULL, _fini)

extern const struct mod_ops __unit_mod_ops;

/**
 * Unit init/fini operations signature.
 *
 * @return error code
 * @retval 0 on success
 * @retval nonzero on operation failure
 */
typedef int (*unit_op_t)(void);

struct unit {
	unit_op_t init;
	unit_op_t fini;
};

#ifdef __CDT_PARSER__

# undef  EMBOX_UNIT
# define EMBOX_UNIT(init, exit) \
	static int init(void) // XXX exit arg.

# undef  EMBOX_UNIT_INIT
# define EMBOX_UNIT_INIT(init) \
	static int init(void)

# undef  EMBOX_UNIT_FINI
# define EMBOX_UNIT_FINI(exit) \
	static int exit(void)

#endif /* __CDT_PARSER__ */

#endif /* EMBOX_UNIT_H_ */

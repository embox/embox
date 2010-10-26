/**
 * @file
 * @brief Unit entity.
 * @details Unit is the simplest and most generic superstructure over the mod
 * entity independent from any frameworks.
 *
 * @date 10.03.2010
 * @author Eldar Abusalimov
 */

#ifndef EMBOX_UNIT_H_
#define EMBOX_UNIT_H_

#include <stddef.h>
#include <embox/kernel.h>
#include <mod/self.h>

#define __EMBOX_UNIT(_init, _fini) \
	static const struct unit __unit = { \
		.init = _init, \
		.fini = _fini, \
	}; \
	MOD_SELF_BIND(&__unit, &__unit_mod_ops)

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
typedef int(*unit_op_t)(void);

struct unit {
	unit_op_t init;
	unit_op_t fini;
};

#endif /* EMBOX_UNIT_H_ */

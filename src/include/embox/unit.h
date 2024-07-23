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
#include <sys/cdefs.h>

#include <framework/mod/self.h>

#define __EMBOX_UNIT(_init, _fini)               \
	const struct unit __MOD(__EMBUILD_MOD__) = { \
	    .mod = MOD_SELF_INIT(&__unit_mod_ops),   \
	    .init = _init,                           \
	    .fini = _fini,                           \
	};                                           \
	MOD_SELF_RUNTIME()

#define EMBOX_UNIT(_init, _fini) \
	static int _init(void);      \
	static int _fini(void);      \
	__EMBOX_UNIT(_init, _fini)

#define EMBOX_UNIT_INIT(_init) \
	static int _init(void);    \
	__EMBOX_UNIT(_init, NULL)

#define EMBOX_UNIT_FINI(_fini) \
	static int _fini(void);    \
	__EMBOX_UNIT(NULL, _fini)

#ifdef __CDT_PARSER__

#undef EMBOX_UNIT
#define EMBOX_UNIT(init, exit) \
	EMBOX_UNIT_INIT(init);     \
	EMBOX_UNIT_FINI(exit)

#undef EMBOX_UNIT_INIT
#define EMBOX_UNIT_INIT(init)                \
	static int init(void);                   \
	typedef typeof(init) __unit_placeholder; \
	static int init(void)

#undef EMBOX_UNIT_FINI
#define EMBOX_UNIT_FINI(exit)                \
	static int exit(void);                   \
	typedef typeof(exit) __unit_placeholder; \
	static int exit(void)

#endif /* __CDT_PARSER__ */

__BEGIN_DECLS

/**
 * Unit init/fini operations signature.
 *
 * @return error code
 * @retval 0 on success
 * @retval nonzero on operation failure
 */
typedef int (*unit_op_t)(void);

struct unit {
	struct mod mod;
	unit_op_t init;
	unit_op_t fini;
};

extern const struct mod_ops __unit_mod_ops;

__END_DECLS

#endif /* EMBOX_UNIT_H_ */

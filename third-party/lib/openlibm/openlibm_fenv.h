/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 03.09.25
 */

#ifndef THIRD_PARTY_LIB_OPENLIBM_FENV_H_
#define THIRD_PARTY_LIB_OPENLIBM_FENV_H_

#include <stdint.h>
#include <sys/cdefs.h>

typedef uint32_t fenv_t;
typedef uint32_t fexcept_t;

#define FE_INVALID   0x0001
#define FE_DIVBYZERO 0x0002
#define FE_OVERFLOW  0x0004
#define FE_UNDERFLOW 0x0008
#define FE_INEXACT   0x0010

#define FE_ALL_EXCEPT \
	(FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

/* Rounding modes */
#define FE_TONEAREST  0x0000
#define FE_TOWARDZERO 0x0001
#define FE_UPWARD     0x0002
#define FE_DOWNWARD   0x0003

/* Default floating-point environment */
#define FE_DFL_ENV (&__fe_dfl_env)

__BEGIN_DECLS

extern const fenv_t __fe_dfl_env;

static inline int feclearexcept(int excepts) {
	return 0;
}

static inline int fegetexceptflag(fexcept_t *flagp, int excepts) {
	return 0;
}

static inline int fesetexceptflag(const fexcept_t *flagp, int excepts) {
	return 0;
}

static inline int feraiseexcept(int excepts) {
	return 0;
}

static inline int fetestexcept(int excepts) {
	return 0;
}

static inline int fegetround(void) {
	return 0;
}

static inline int fesetround(int round) {
	return 0;
}

static inline int fegetenv(fenv_t *envp) {
	return 0;
}

static inline int feholdexcept(fenv_t *envp) {
	return 0;
}

static inline int fesetenv(const fenv_t *envp) {
	return 0;
}

static inline int feupdateenv(const fenv_t *envp) {
	return 0;
}

__END_DECLS

#endif /* !THIRD_PARTY_LIB_OPENLIBM_FENV_H_ */

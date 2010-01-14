/**
 * @file
 * @brief Provides @c assert macro definition
 *
 * @date 25.11.09
 * @author Eldar Abusalimov
 * @author Nikolay Korotky
 */

#ifndef ASSERT_H_
#define ASSERT_H_

#ifdef _TEST_SYSTEM_
#include <autoconf.h>
#include <types.h>
#endif

//#include <hal/arch.h>

#if !defined(DEBUG) && defined(_TEST_SYSTEM_)
# define __ASSERT_STRING0(cond, file, line) \
		"\nASSERTION FAILED at " #file " : " #line "\n" \
		"(" cond ") is not true\n"

# define __ASSERT_STRING(cond, file, line) \
		__ASSERT_STRING0(cond, file, line)

# define assert(cond) \
		do if (!(cond)) { \
			puts(__ASSERT_STRING(#cond, __FILE__, __LINE__)); \
			/*arch_shutdown(ARCH_SHUTDOWN_MODE_DUMP);*/ \
		} while(0)
#else
# define assert(cond)   do ; while(0)
#endif /* DEBUG */

#define assert_null(arg) assert((arg) == NULL)
#define assert_not_null(arg) assert((arg) != NULL)
#define ASSERT_INIT_DONE() assert_not_null(dev_regs)

#endif /* ASSERT_H_ */

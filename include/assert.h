/**
 * @file assert.h
 * @brief Provides @c assert macro definition
 *
 * @date 25.11.09
 * @author Eldar Abusalimov
 * @author sikmir
 */

#ifndef ASSERT_H_
#define ASSERT_H_

#include <autoconf.h>
//#include <hal/machine.h>
#include <types.h>

#ifndef DEBUG
# define __ASSERT_STRING0(cond, file, line) \
		"\nASSERTION FAILED at " #file " : " #line "\n" \
		"(" cond ") is not true\n"

# define __ASSERT_STRING(cond, file, line) \
		__ASSERT_STRING0(cond, file, line)

# define assert(cond) \
		do if (!(cond)) { \
			puts(__ASSERT_STRING(#cond, __FILE__, __LINE__)); \
			/*machine_panic();*/ \
		} while(0)
#else
# define assert(cond)   do ; while(0)
#endif /* DEBUG */

#define assert_null(arg) assert((arg) == NULL)
#define assert_not_null(arg) assert((arg) != NULL)

#endif /* ASSERT_H_ */

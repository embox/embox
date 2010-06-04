/**
 * @file
 * @brief Some widely used macro definitions.
 *
 * @date 30.05.2010
 * @author Eldar Abusalimov
 */

#ifndef UTIL_MACRO_H_
#define UTIL_MACRO_H_

#define MACRO_STRING(m) #m

#define MACRO_CONCAT(m1, m2) m1 ## m2

#define MACRO_EXPAND(m) m

#define MACRO_INVOKE(m, ...) m(__VA_ARGS__)

#endif /* UTIL_MACRO_H_ */

/**
 * @file
 * @brief Some widely used macro definitions.
 *
 * @date 30.05.10
 * @author Eldar Abusalimov
 */

#ifndef UTIL_MACRO_H_
#define UTIL_MACRO_H_

/**
 * Indirect stringification of the specified macro.
 *
 * @param m
 *  Macro to stringify
 * @return
 *  String literal containing expansion of the specified macro
 */
#define MACRO_STRING(m) \
		__MACRO_STRING(m)
#define __MACRO_STRING(m) \
	# m

/**
 * Indirect concatenation of two macros.
 *
 * @param m1
 *  The first macro
 * @param m2
 *  The second macro
 * @return
 *  Concatenated expansions of the specified macros
 */
#define MACRO_CONCAT(m1, m2) \
		__MACRO_CONCAT(m1, m2)
#define __MACRO_CONCAT(m1, m2) \
	m1 ## m2

/**
 * Just expands a macro argument.
 *
 * @param m
 *  The macro to expand
 * @return
 *  Macro expansion
 */
#define MACRO_EXPAND(m) \
	m

/**
 * Expands macro function with the specified parameters.
 *
 * @param m
 *  The macro to expand
 * @param ...
 *  Arguments to pass to the macro function
 * @return
 *  Expansion of @code m(...) @endcode
 */
#define MACRO_INVOKE(m, ...) \
	m(__VA_ARGS__)

/**
 * Appends a guard (namely current line number) to the specified symbol.
 *
 * @param symbol
 *  Symbol (or macro which expand to symbol) to protect
 * @return
 *  Symbol suffixed with the guard
 */
#define MACRO_GUARD(symbol) \
		__MACRO_GUARD(symbol)
#define __MACRO_GUARD(symbol) \
	MACRO_CONCAT(symbol ## _at_line_, __LINE__)

#endif /* UTIL_MACRO_H_ */

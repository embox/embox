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
#define MACRO_STRING(...) __MACRO_STRING(__VA_ARGS__)
#define __MACRO_STRING(...) # __VA_ARGS__

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
#define MACRO_CONCAT(m1, m2) __MACRO_CONCAT(m1, m2)
#define __MACRO_CONCAT(m1, m2) m1 ## m2

/**
 * Just expands a macro argument.
 *
 * @param m
 *  The macro to expand
 * @return
 *  Macro expansion
 */
#define MACRO_EXPAND(m) m

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
#define MACRO_INVOKE(m, ...) m(__VA_ARGS__)

/**
 * Appends a guard (namely current line number) to the specified symbol.
 *
 * @param symbol
 *  Symbol (or macro which expand to symbol) to protect
 * @return
 *  Symbol suffixed with the guard
 */
#define MACRO_GUARD(symbol) __MACRO_GUARD(symbol)
#define __MACRO_GUARD(symbol) MACRO_CONCAT(symbol ## _at_line_, __LINE__)

/**
 * Apply macro function f(a, tail) for each argument recursively.
 *
 * @param f
 *  Macro function
 * @param ...
 *  List of arguments
 * @return
 *  Macro expansion
 */
#define MACRO_FOREACH(f, ...) \
	__MACRO_FOREACH_N(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1)(f, __VA_ARGS__)
#define __MACRO_FOREACH_N(_9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) \
	MACRO_CONCAT(__MACRO_FOREACH_, N)
#define __MACRO_FOREACH_1(f, a)      f(a, )
#define __MACRO_FOREACH_2(f, a, ...) f(a, __MACRO_FOREACH_1(f, __VA_ARGS__))
#define __MACRO_FOREACH_3(f, a, ...) f(a, __MACRO_FOREACH_2(f, __VA_ARGS__))
#define __MACRO_FOREACH_4(f, a, ...) f(a, __MACRO_FOREACH_3(f, __VA_ARGS__))
#define __MACRO_FOREACH_5(f, a, ...) f(a, __MACRO_FOREACH_4(f, __VA_ARGS__))
#define __MACRO_FOREACH_6(f, a, ...) f(a, __MACRO_FOREACH_5(f, __VA_ARGS__))
#define __MACRO_FOREACH_7(f, a, ...) f(a, __MACRO_FOREACH_6(f, __VA_ARGS__))
#define __MACRO_FOREACH_8(f, a, ...) f(a, __MACRO_FOREACH_7(f, __VA_ARGS__))
#define __MACRO_FOREACH_9(f, a, ...) f(a, __MACRO_FOREACH_8(f, __VA_ARGS__))

#endif /* UTIL_MACRO_H_ */

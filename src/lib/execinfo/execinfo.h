/**
 * @file
 * @brief man BACKTRACE(3)
 *
 * @date 25.08.12
 * @author Anton Bulychev
 * @author Ilia Vaprol
 */

#ifndef EXECINFO_H_
#define EXECINFO_H_

extern int backtrace(void **buff, int size);
extern char **backtrace_symbols(void *const *buff, int size);
extern void backtrace_symbols_fd(void *const *buff, int size, int fd);

/**
 * @brief Gives string representation of ptr. If function is found, it's
 * in "0xabcdefff <function_name+offset>" form, and it's "__unknown__'
 * otherwise.
 *
 * @param ptr Pointer to code
 * @param out Pointer to string buffer where result will be stored
 * @param size Size of @a out buffer
 *
 * @return Number of bytes written to @a out
 */
extern int backtrace_symbol_buf(void *ptr, char *out, int size);

#endif /* EXECINFO_H_ */

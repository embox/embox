/**
 * @file
 * @brief
 *
 * @date 25.08.12
 * @author Anton Bulychev
 * @author Ilia Vaprol
 */

#ifndef EXECINFO_H_
#define EXECINFO_H_

extern int backtrace(void **buff, int size);
extern char ** backtrace_symbols(void *const *buff, int size);
extern void backtrace_symbols_fd(void *const *buff, int size, int fd);

#endif /* EXECINFO_H_ */

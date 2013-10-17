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

int backtrace(void **buff, int size);
char ** backtrace_symbols(void *const *buff, int size);
void backtrace_symbols_fd(void *const *buff, int size, int fd);

#endif /* EXECINFO_H_ */

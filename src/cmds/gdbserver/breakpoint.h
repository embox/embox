/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.12.22
 */

#ifndef BREAKPOINT_H_
#define BREAKPOINT_H_

#include <stdbool.h>

/**
 * @brief Insert software breakpoint at address
 * @retval On success: true
 * @retval On error: false
 */
extern bool insert_breakpoint(void *addr);

/**
 * @brief Remove software breakpoint from address
 * @retval On success: true
 * @retval On error: false
 */
extern bool remove_breakpoint(void *addr);

/**
 * @brief Remove all software breakpoints
 */
extern void remove_all_breakpoints(void);

#endif /* BREAKPOINT_H_ */

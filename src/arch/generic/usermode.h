/**
 * @file
 * @brief
 *
 * @date 12.10.2012
 * @author Anton Bulychev
 */

#ifndef ARCH_USERMODE_H_
#define ARCH_USERMODE_H_

/* We don't have usermode, so just calling function. */
#define usermode_call_and_switch_if(cond,func,arg) func(arg)

#endif /* ARCH_USERMODE_H_ */

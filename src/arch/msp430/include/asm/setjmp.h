/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.08.2013
 */

#ifndef MSP430_SETJMP_H_
#define MSP430_SETJMP_H_

#define _JBLEN 20

#ifndef __ASSEMBLER__

typedef	int __jmp_buf[_JBLEN];

#endif

#endif /* MSP430_SETJMP_H_ */

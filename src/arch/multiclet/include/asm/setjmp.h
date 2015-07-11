/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    12.07.2015
 */

#ifndef MULTICLET_SETJMP_H_
#define MULTICLET_SETJMP_H_

#define _JBLEN 128 /* value taken to be reasonable big random number */

#ifndef __ASSEMBLER__

typedef int __jmp_buf[_JBLEN];

#endif

#endif /* MULTICLET_SETJMP_H_ */

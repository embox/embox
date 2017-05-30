/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    22.03.2017
 */

#ifndef ESP8266_SETJMP_H_
#define ESP8266_SETJMP_H_

#define _JBLEN 20

#ifndef __ASSEMBLER__

typedef	int __jmp_buf[_JBLEN];

#endif

#endif /* ESP8266_SETJMP_H_ */

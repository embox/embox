/**
 * @file
 *
 * @date Aug 29, 2013
 * @author: Anton Bondarev
 */

#ifndef SIZE_T_DEF_H_
#define SIZE_T_DEF_H_

#ifdef __SIZE_TYPE__
 typedef __SIZE_TYPE__ size_t;
#else
 #include <asm/types.h>
 typedef __size_t size_t;
#endif


#endif /* SIZE_T_DEF_H_ */

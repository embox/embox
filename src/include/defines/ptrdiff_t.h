/**
 * @file
 *
 * @date Oct 17, 2013
 * @author: Anton Bondarev
 */

#ifndef PTRDIFF_T_DEF_H_
#define PTRDIFF_T_DEF_H_


#ifdef __PTRDIFF_TYPE__
 typedef __PTRDIFF_TYPE__ ptrdiff_t;
#else
 #include <asm/types.h>
 typedef __ptrdiff_t ptrdiff_t;
#endif

#endif /* PTRDIFF_T_DEF_H_ */

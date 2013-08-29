/**
 * @file
 *
 * @date Aug 29, 2013
 * @author: Anton Bondarev
 */

#ifndef NULL_DEF_H_
#define NULL_DEF_H_

#undef NULL

#ifndef __cplusplus
 #define NULL ((void *)0)
#else /* __cplusplus */
 #define NULL 0
#endif /* ! __cplusplus */


#endif /* NULL_DEF_H_ */

/**
 * @file
 *
 * @date Sep 30, 2013
 * @author: Anton Bondarev
 */

#ifndef CLOCKID_T_DEF_H_
#define CLOCKID_T_DEF_H_

#ifdef __CLOCKID_T_TYPE
 typedef __CLOCKID_T_TYPE clockid_t;
#else
 typedef int clockid_t;
#endif

#endif /* CLOCKID_T_DEF_H_ */

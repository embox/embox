/**
 * @file
 *
 * @date Sep 30, 2013
 * @author: Anton Bondarev
 */

#ifndef TIME_T_DEF_H_
#define TIME_T_DEF_H_
#ifdef __TIME_T_TYPE
 typedef __TIME_T_TYPE time_t;
#else
 typedef long time_t;
#endif

#endif /* TIME_T_DEF_H_ */

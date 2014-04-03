/**
 * @file
 *
 * @date Sep 30, 2013
 * @author: Anton Bondarev
 */

#ifndef CLOCK_T_DEF_H_
#define CLOCK_T_DEF_H_

#ifdef __CLOCK_TYPE__
 typedef __CLOCK_TYPE__ clock_t;
#else
 typedef unsigned long clock_t;
#endif


#endif /* CLOCK_T_DEF_H_ */

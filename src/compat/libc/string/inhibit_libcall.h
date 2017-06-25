/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.07.2015
 */

#ifndef STR_INHIBIT_LIBCALL_H_
#define STR_INHIBIT_LIBCALL_H_

#ifdef HAVE_CC_INHIBIT_LOOP_TO_LIBCALL
# define inhibit_loop_to_libcall \
	    __attribute__ ((__optimize__ ("-fno-tree-loop-distribute-patterns")))
#else
# define inhibit_loop_to_libcall
#endif

#endif /* STR_INHIBIT_LIBCALL_H_ */

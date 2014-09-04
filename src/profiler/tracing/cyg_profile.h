/*
 *	@author Denis Deryugin
 *	@date 3 Sep 2014
 */

#ifndef CYG_PROFILE_H_
#define CYG_PROFILE_H

typedef void (*cyg_func)(void *, void *);

extern void __cyg_profile_func_enter(void *func, void *caller);
extern void __cyg_profile_func_exit(void *func, void *caller);

#endif /* CYG_PROFILE_H_ */

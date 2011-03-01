/**
 * @file
 * @brief simple pseudo process, that allows to have copies of variables
 *
 * @author Fedor Burdun
 * @date 11.12.10
 */

#ifndef PSEUDO_PROCESS_H_
#define PSEUDO_PROCESS_H_

#ifndef PP_VAR
#define PP_VAR( name )
#endif

#include <kernel/thread.h>
#include <drivers/tty.h>
#include <stdio.h>

#ifdef CONFIG_PP_TEST
extern char share_variable;
#endif

#define PP_INIT_LIST \
	PP_VAR( cur_console ) /* from: stdio.c */ \
	PP_VAR( share_variable ) /* from: test_pp.c */ \

#define PP_SIZEOF_LIST \
	sizeof(cur_console) + \
	sizeof(char) // share_variable

#define PP_INIT_LIST_S     2
#define PP_THREADS_S       10

typedef struct pprocess {
	struct thread  thread[PP_THREADS_S];
	uint8_t        data[PP_SIZEOF_LIST];
} pprocess_t;

//extern int		pp_init();
//extern void    *pp_pool[];
//extern size_t   pp_pool_sz[];
//extern uint32_t pp_pool_s;
extern void     pp_store(struct pprocess* pr);
extern void     pp_restore(struct pprocess* pr);

extern void        pp_add_thread(struct pprocess *p, struct thread *th);
extern void        pp_del_thread(struct pprocess *p, struct thread *th);
extern struct pprocess* pp_create( void (*run)(void) ); /* with dynamic allocation */
extern struct pprocess* pp_create_ws( void (*run)(void), void *stack_addr);
extern struct pprocess* pp_add_process(struct thread *th);
extern void        pp_del_process(struct pprocess *p);
extern void        pp_switch_process(struct pprocess *p);
extern struct pprocess  *pp_cur_process;

#endif /* PSEUDO_PROCESS_H_ */


/**
 * @file
 *
 * @brief simple pseudo process, that allows to have copies of variables
 *
 * @author Fedor Burdun
 * @date 11.12.10
 */
#include <types.h>
#include <string.h>

#define PP_VAR( var ) \
	pp_pool[ pp_pool_s ] = &var; \
	pp_pool_sz[ pp_pool_s ] = sizeof(var); \
	++pp_pool_s;
#if 0

	printf("pp_pool_s: %d\n",pp_pool_s);\
	printf("sizeof(%s)=%d\n",#var,sizeof(var));\
	printf("ptr(%s)=%d\n",#var,&var);\
	printf("pp_pool_sz[%d]=%d\n",pp_pool_s,pp_pool_sz[pp_pool_s]);
#endif


#include <embox/unit.h>
#include <kernel/pp.h>
#include <kernel/mm/slab.h>

#define CONFIG_PP_COUNT 10

#ifdef CONFIG_PP_TEST
char *share_variable;
#endif

static void* pp_pool[PP_INIT_LIST_S];
size_t pp_pool_sz[PP_INIT_LIST_S];
static uint32_t pp_pool_s = 0;
static bool pp_has_init = false;
pprocess_t *pp_cur_process;

#define NO_KMEM_USE
#ifdef KMEM_USE
ADD_CACHE(pp_p,pprocess_t,CONFIG_PP_COUNT)
#else
#define CONFIG_PP_COUNT 10
static struct pprocess pp_process_pool[CONFIG_PP_COUNT];
static uint32_t pp_process_pool_c = 0;

struct pprocess* get_more_pp(void) {
	if (pp_process_pool_c>=CONFIG_PP_COUNT) {
		LOG_ERROR("ERROR: Not enough pool size for pprocess. C:%d, S:%d",pp_process_pool_c,CONFIG_PP_COUNT);
		while (true);
	}
	return &pp_process_pool[pp_process_pool_c++];
}
#endif



static int pp_init(void) {
#if 0
	void *tmp = &cur_console;
	int si = sizeof(cur_console);

	int i;
#endif

	printk("PP_INIT: ");

	pp_pool_s = 0;
	PP_INIT_LIST();
	//FIXME PProcess must use kmem interface
	#ifdef KMEM_USE
	pp_cur_process = kmem_cache_alloc(pp_p);
	#else
	pp_cur_process = get_more_pp();
	#endif
	pp_has_init = true;
	printk(" [ done ]\n");
	return 0;
}

void pp_store( struct pprocess * pr ) {
	uint32_t i;
	size_t c = 0;

	if (!pp_has_init) {
		pp_init();
		pp_has_init = true;
	}

	if ( pr == NULL ) {
		return;
	}

	for (i=0; i<pp_pool_s; ++i) {
		memcpy(&pr->data[c], pp_pool[i], pp_pool_sz[i]);
		c+=pp_pool_sz[i];
	}
}

void pp_restore( struct pprocess* pr ) {
	uint32_t i;
	size_t c = 0;

	if (!pp_has_init) {
		LOG_ERROR("Try pp_restore, but pool has not initialized.");
		pp_init();
		pp_has_init = true;
	}

	if ( pr == NULL ) {
		return;
	}

	for (i=0; i<pp_pool_s; ++i) {
		memcpy(pp_pool[i],&pr->data[c],pp_pool_sz[i]);
		c+=pp_pool_sz[i];
	}
}

void pp_add_thread( pprocess_t *p, struct thread *th ) {
	th->pp = p;
}

void pp_del_thread( pprocess_t *p, struct thread *th ) {
}

pprocess_t* pp_add_process( struct thread *th ) {
	#ifdef KMEM_USE
	pprocess_t *p = kmem_cache_alloc(pp_p);
	#else
	pprocess_t *p = get_more_pp();
	#endif

	if (NULL == p) {
		LOG_ERROR("Pseudo process ERROR: no space in pool for process");
		return NULL;
	}
	pp_add_thread( p , th );
	return p;
}

void pp_del_process( struct pprocess *p ) {
}

void pp_switch_process( struct pprocess *p ) {
	pp_cur_process = p;
}

struct pprocess* pp_create_ws( void (*run)(void), void *stack_addr) {
	struct thread *t;
	struct pprocess *p;

	if (!(t = thread_alloc())) {
		return NULL;
	}
	thread_init(t, run, stack_addr);
	t->priority = THREAD_PRIORITY_MAX;

	p = pp_add_process(t);
	pp_store(p);

	thread_start(t);

	return p;
}

EMBOX_UNIT_INIT(pp_init);



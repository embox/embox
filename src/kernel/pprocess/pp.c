/**
 * @file
 *
 * @brief simple pseudo process, that allows to have copies of variables
 *
 * @author Fedor Burdun
 * @date 11.12.10
 */

#define PP_VAR( var ) \
	pp_pool_sz[ pp_pool_s ] = sizeof(var); \
	pp_pool[ pp_pool_s ++ ] = &var;

#include <kernel/pp.h>
#include <kernel/mm/slab.h>

#define CONFIG_PP_COUNT 10

#ifdef CONFIG_PP_TEST
static char share_variable;
#endif

static void* pp_pool[PP_INIT_LIST_S];
static size_t pp_pool_sz[PP_INIT_LIST_S];
static uint32_t pp_pool_s = 0;
static bool pp_has_init = false;
static pprocess_t *pp_cur_process;

#define NO_KMEM_USE
#ifdef KMEM_USE
ADD_CACHE(pp_p,pprocess_t,CONFIG_PP_COUNT)
#else
#define PP_PR_S 4
static struct pprocess pp_process_pool[PP_PR_S];
static uint32_t pp_process_pool_c = 0;

struct pprocess* get_more_pp() {
	if (pp_process_pool_c>=PP_PR_S) {
		LOG_ERROR("ERROR: Not enough pool size for pprocess.");
		while(true);
	}
	return &pp_process_pool[pp_process_pool_c++];
}
#endif

void pp_init() {
	PP_INIT_LIST
	#ifdef KMEM_USE
	pp_cur_process = kmem_cache_alloc(pp_p);
	#else
	pp_cur_process = get_more_pp();
	#endif
}

void pp_store( void* data ) {
	uint32_t i;
	size_t c;

	if (!pp_has_init) {
		pp_init();
		pp_has_init = true;
	}

	for (i=0; i<pp_pool_s; ++i) {
		memcpy(&data[c], pp_pool[i], pp_pool_sz[i]);
		c+=pp_pool_sz[i];
	}
}

void pp_restore( void* data ) {
	uint32_t i;
	size_t c;

	if (!pp_has_init) {
		LOG_ERROR("Try pp_restore, but pool has not initialized.");
		pp_init();
		pp_has_init = true;
	}

	for (i=0; i<pp_pool_s; ++i) {
		memcpy(pp_pool[i],&data[c],pp_pool_sz[i]);
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

	if (p==NULL) {
		LOG_ERROR("Pseudo process ERROR: no space in pool for process");
		return NULL;
	}
	pp_add_thread( p , th );
}

void pp_del_process( struct pprocess *p ) {
}

void pp_switch_process( struct pprocess *p ) {
	pp_cur_process = p;
}



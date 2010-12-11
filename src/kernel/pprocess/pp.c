/**
 * @file
 *
 * @brief simple pseudo process, that allows to have copies of variables
 *
 * @author Fedor Burdun
 * @date 11.12.10
 */

#define PP_VAR( var ) \
	pp_pool_sz[ pool_s ] = sizeof(var); \
	pp_pool[ pool_s ++ ] = &var;

#include <kernel/pp.h>
#include <kernel/mm/slab.h>

#define CONFIG_PP_COUNT 10

static void* pp_pool[PP_INIT_LIST_S];
static size_t pp_pool_sz[PP_INIT_LIST_S];
static uint32_t pp_pool_s = 0;
static bool pp_has_init = false;
static pprocess_t *pp_cur_process;
ADD_CACHE(pp_p,pprocess_t,CONFIG_PP_COUNT)

void pp_init() {
	PP_INIT_LIST
	cur_process = kmem_cache_alloc(pp_p);
}

void pp_store( void* data ) {
	uint32_t i;
	size_t c;

	if (!pp_has_init) {
		pp_init();
		pp_has_init = true;
	}

	for (i=0; i<pp_pool_s; ++i) {
		data[c] = pp_pool[i];
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
		pp_pool[i] = data[c];
		c+=pp_pool_sz[i];
	}
}

void pp_add_thread( pprocess_t *p, struct thread *th ) {
	th->pp = p;
}

void pp_del_thread( pprocess_t *p, struct thread *th ) {
}

pprocess_t* pp_add_process( struct thread *th ) {
	process_t *p = kmem_cache_alloc(pp_p);
	if (p==NULL) {
		LOG_ERROR("Pseudo process ERROR: no space in pool for process");
		return NULL;
	}
	pp_add_thread( p , th );
}

void pp_del_process( pprocess_t *p ) {
}

void pp_switch_process( process_t *p ) {
	pp_cur_process = p;
}



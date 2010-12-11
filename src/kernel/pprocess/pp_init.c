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

static void* pp_pool[PP_INIT_LIST_S];
static size_t pp_pool_sz[PP_INIT_LIST_S];
static uint32_t pp_pool_s = 0;

void pp_init() {
	PP_INIT_LIST
}

void pp_store( void* data ) {
	uint32_t i;
	size_t c;
	for (i=0; i<pp_pool_s; ++i) {
		data[c] = pp_pool[i];
		c+=pp_pool_sz[i];
	}
}

void pp_restore( void* data ) {
	uint32_t i;
	size_t c;
	for (i=0; i<pp_pool_s; ++i) {
		pp_pool[i] = data[c];
		c+=pp_pool_sz[i];
	}
}


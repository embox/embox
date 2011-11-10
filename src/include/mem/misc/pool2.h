/**
 * @file	pool2.h
 *
 * @brief	Object pool allocator, new implementation
 *
 * @date	10.11.11
 *
 * @author	Gleb Efimov
 */

#ifndef MEM_MISC_POOL2_H_
#define MEM_MISC_POOL2_H_

#include <types.h>
#include <util/slist.h>

struct obj_pool {
	size_t object_size;
	size_t size;
	int cnt;
	int free_cnt;
	struct slist list_free;
	char *mem;
};

const size_t OBJECT_SIZE = 1000;

struct pool_obj;

#define POOL_DEF(pool_name,pool_size)				 				\
		struct obj_pool  pool = {				  	 				\
			.object_size = OBJECT_SIZE,			  	 				\
			.size = pool_size,                   	 				\
			.free_cnt = 0,						  	 				\
			.cnt = 0,								 				\
			.list_free = slist_init(&list_free),  	 				\
			.&mem = malloc(OBJECT_SIZE * pool_size * sizeof(char))  \
		};										     				\

extern void *pool2_alloc(struct obj_pool *pool);

extern void pool2_free(struct obj_pool *pool, struct pool_obj *obj);

#endif /* MEM_MISC_POOL2_H_ */

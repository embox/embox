/**
 * @file
 * @brief
 *
 * @date 27.12.12
 * @author Alexander Kalmuk
 */

#include <javacall_memory.h>
#include <stdlib.h>

void* javacall_memory_heap_allocate(long size, /*OUT*/ long* outSize) {
	void *heap = malloc(size);

	if (!heap) {
		*outSize = 0;
	} else {
		*outSize = size;
	}

	return heap;
}

void javacall_memory_heap_deallocate(void* heap) {
	free(heap);
}

void* /*OPTIONAL*/ javacall_malloc(unsigned int size){
	return malloc(size) ;
}

void  /*OPTIONAL*/ javacall_free(void* ptr) {
	free(ptr);
}

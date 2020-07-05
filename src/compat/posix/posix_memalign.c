/**
 * @file
 * @brief
 *
 * @author  KOUAM Ledoux
 * @date    04.07.2020
 */
#include <errno.h>
#include <unistd.h>
#include  <vmem/vmem.h>
/*
    * The  function  posix_memalign() allocates size bytes and places the address of the allocated memory in *__memptr.  
    * The address of the allocated memory will be a multiple of __alignment, which must be a power of two and a multiple of sizeof(void *).  
    * If __size is 0, then the value placed in  *__memptr is either NULL, or a unique pointer value that can later be successfully passed to free().
*/
int power_of_two(int nmber) {
    while (nmber %2 == 0) {
        nmber /= 2 ;
    }
	if (nmber == 1) {
        return 0 ;
    } else {
        return 1 ;
    }
}
int posix_memalign (void **__memptr, size_t __alignment, size_t __size) {
    if (power_of_two(__alignment) != 0 || __size == 0){
        return EINVAL;
    }
    *__memptr = memalign(__alignment,  __size) ;
    if (*__memptr == NULL) {
        return ENOMEM ;
    }
    return 0;
}
void *aligned_alloc(size_t alignment, size_t size) {
    //that size should be a multiple of alignment.
    if (alignment == 0 || size == 0) {
        return NULL ;
    } else if (size % alignment==0) {
        return memalign(alignment, size) ;
    } else {
        return NULL ;
    }
}
void *valloc(size_t size) {
    if (size == 0) {
        return NULL ;
    } else {
       return memalign(MMU_PAGE_SIZE, size) ;
    }
}
void *pvalloc(size_t size) {
    /*
        * rounds the size 
        * of the allocation up to the next multiple of the system page size
    */
    if (size == 0) {
       return NULL ;
    } else if ((size % MMU_PAGE_SIZE) == 0) {
       return valloc(size) ;
    } else {
       size /= MMU_PAGE_SIZE ;
       size++;
       size *= MMU_PAGE_SIZE ;
       return valloc(size) ;
   }
}

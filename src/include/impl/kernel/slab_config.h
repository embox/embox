/**
 * @file
 * @brief Defines constants for slab allocator
 *
 * @date 26.02.2011
 * @author Alexandr Kalmuk
 */

/* Length of name of any cache */
#define CACHE_NAMELEN 16
/* max slab size in 2^n form */
#define MAX_GFP_ORDER 3
/* max object size in 2^n form */
#define MAX_OBJ_ORDER 3
/* number for defining acceptable internal fragmentation */
#define MAX_INTFRAGM_ORDER 8
/* size of kmalloc_cache in pages */
#define KMALLOC_CACHE_SIZE 1

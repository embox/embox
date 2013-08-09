/**
 * @file
 * @brief
 *
 * @date 30.07.2013
 * @author Andrey Gazukin
 */

#ifndef __LINUX_SLAB_H__
#define __LINUX_SLAB_H__

#include <stdlib.h>

#include <asm/page.h> /* Don't ask. Linux headers are a mess. */

#define kmalloc(x, y) malloc(x)
#define kfree(x) free(x)
#define vmalloc(x) malloc(x)
#define vfree(x) free(x)

#endif /* __LINUX_SLAB_H__ */


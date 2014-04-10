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

#define kmalloc(x, y) kmalloc(x)
#define kfree(x) kfree(x)
#define vmalloc(x) kmalloc(x)
#define vfree(x) kfree(x)

#endif /* __LINUX_SLAB_H__ */


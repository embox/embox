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
#include <mem/sysmalloc.h>

#define kmalloc(x, y) sysmalloc(x)
#define kfree(x) sysfree(x)
#define vmalloc(x) sysmalloc(x)
#define vfree(x) sysfree(x)

#endif /* __LINUX_SLAB_H__ */


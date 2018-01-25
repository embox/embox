/**
 * @file
 * @brief
 *
 * @date 29.07.2013
 * @author Andrey Gazukin
 */

#ifndef __LINUX_PAGEMAP_H__
#define __LINUX_PAGEMAP_H__

#include <asm/bug.h>
#include <asm/page.h>
#include <mem/sysmalloc.h>

#define PAGE_CACHE_SHIFT        PAGE_SHIFT
#define PAGE_CACHE_SIZE         PAGE_SIZE()

#define GFP_KERNEL    0
#define GFP_NOWAIT    1

#define PageLocked(pg) 1
#define Page_Uptodate(pg) 0
#define UnlockPage(pg)
#define PAGE_BUG(pg) BUG()
#define ClearPageUptodate(pg)
#define SetPageError(pg)
#define ClearPageError(pg)
#define SetPageUptodate(pg)

#endif /* __LINUX_PAGEMAP_H__ */

/**
 * @file
 * @brief
 *
 * @date 29.07.2013
 * @author Andrey Gazukin
 */

#ifndef __ASM_PAGE_H__
#define __ASM_PAGE_H__

#include <mem/page.h>

#define PAGE_SHIFT   12

/* These aren't used by much yet. If that changes, you might want
   to make them actually correct :) 
   
   kpishere: Correct for ASMV6 */
#ifndef PAGE_SIZE
#define PAGE_SIZE  PAGE_SIZE()  /* (0x1 << PAGE_SHIFT) */
#endif

#endif /* __ASM_PAGE_H__ */

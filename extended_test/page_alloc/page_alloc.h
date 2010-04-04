/**
 * @file
 * @brief page allocator header
 * @details
 *
 * @date 04.04.10
 * @author Fedor Burdun
 */

#ifndef __PAGE_ALLOC_H_
#define __PAGE_ALLOC_H_

#ifdef DEBUG_x86_ONLY /* Code included for debug specific */
#include <malloc.h>
#else /* Embox specific code */
#endif /* End of specific code */

struct pmark_t;

#ifdef DEBUG_x86_ONLY /* Code included for debug specific */

typedef long psize_t;
typedef struct pmark_t* paddr_t;
const psize_t MAXPAGECOUNT 	= 0xffff;
const paddr_t SIZEOFPAGE	= 0x00ff;
struct paddr_t *cmark_p;
void *mem;
int palloc_hasinit		= 0x0000;

#else /* Embox specific code */

#endif /* End of specific code */

/* structure of page marker */
struct pmark_t {
	psize_t	psize;
	paddr_t *pnext;
	paddr_t *pprev;
};

/* */
paddr_t page_alloc(psize_t);
void 	page_free(paddr_t);

#endif /* __PAGE_ALLOC_H_ */


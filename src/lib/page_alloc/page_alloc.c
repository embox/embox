/**
 * @file
 * @brief page allocator
 * @details
 *
 * @date 04.04.10
 * @author Fedor Burdun
 */

#include <lib/page_alloc.h>

#define PAGE_QUANTITY 0x100
#define PAGE_SIZE 0x100

int page_alloc_hasinit = 0;

#ifdef EXTENDED_TEST
static uint8_t page_pool[PAGE_SIZE][PAGE_QUANTITY];
static pmark_t *cmark_p = (pmark_t *) page_pool;
#else
#define START_MEMORY_ADDR 0x40000000
static pmark_t *cmark_p = (pmark_t *)START_MEMORY_ADDR;
#endif

/* defragmentation pages */
static void page_defrag(void ) {
}

/* copy mark structure */
static pmark_t *copy_mark( pmark_t *from , pmark_t *to ) {
	to->psize = from->psize;
	to->pnext = from->pnext;
	to->pprev = from->pprev;
	return to;
}

/* Initialize page allocator */
int page_alloc_init(void) {
	cmark_p->psize = PAGE_QUANTITY;
	cmark_p->pnext	= cmark_p;
	cmark_p->pprev	= cmark_p;
	return 0;
}

/* allocate page */
pmark_t *page_alloc(void) {
	size_t psize = 1;
	pmark_t *pcur;

	if (!page_alloc_hasinit) {
		page_alloc_init();
		page_alloc_hasinit = 1;
	}
	/* find first proper block */
	pcur = cmark_p->pnext;
	while ( pcur != cmark_p /*&& pcur->psize < psize */) {
		pcur = pcur->pnext;
	}
	/* check finded block */
	if ( pcur->psize >= psize ) {
		/* change list and return value */
		if (pcur->psize > psize ) {
			pcur->psize = pcur->psize - psize;
			cmark_p = copy_mark( pcur , pcur + PAGE_SIZE * psize );
			return pcur;
		} else {
			/* psize == pcur->psize */
			pcur->pprev->pnext = pcur->pnext;
			cmark_p = pcur->pnext;
			return pcur;
		}

	}

	return NULL;
}

/* free page that was allocated */
void page_free(pmark_t *paddr) {
	/* find */
	paddr->psize = 1;
	paddr->pprev = cmark_p->pprev;
	cmark_p->pprev = paddr;
	paddr->pnext = cmark_p;
}


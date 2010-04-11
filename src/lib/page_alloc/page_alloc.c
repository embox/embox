/**
 * @file
 * @brief page allocator
 * @details
 *
 * @date 04.04.10
 * @author Fedor Burdun
 */

/*
 * TODO:
 * 	debug it
 * 		page_alloc don't work
 *
 * 	rewrite it all!!!!!
 */

#include <lib/page_alloc.h>

#ifndef PAGE_QUANTITY
# define PAGE_QUANTITY 0x100
#endif
#ifndef PAGE_SIZE
# define PAGE_SIZE 0x100
#endif

int page_alloc_hasinit = 0;

#ifdef EXTENDED_TEST

//very TEMP!!!
//	#include <stdio.h>
static uint8_t page_pool[PAGE_SIZE][PAGE_QUANTITY];
static pmark_t *cmark_p = (pmark_t *) page_pool;
#else
#define START_MEMORY_ADDR 0x40000000
static pmark_t *cmark_p = (pmark_t *)START_MEMORY_ADDR;
#endif

#ifdef EXTENDED_TEST
pmark_t* get_cmark_p() {
	return cmark_p;
}
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
#if 0
	pmark_t *cmark_pn = cmark_p + PAGE_SIZE;
#endif

	cmark_p->psize = PAGE_QUANTITY;
	cmark_p->pnext	= cmark_p;
	cmark_p->pprev	= cmark_p;

#if 0
	cmark_pn->psize = PAGE_QUANTITY-1;
	cmark_pn->pnext = cmark_p;
	cmark_pn->pprev = cmark_p;
#endif
	return 0;
}

/* allocate page */
pmark_t *page_alloc(void) { /* Don't work!!!! */
	size_t psize = 1;
	int has_only_one_block = 0;

	pmark_t *pcur,*tmp;

	if (!page_alloc_hasinit) {
		page_alloc_init();
		page_alloc_hasinit = 1;
	}
	/* find first proper block */
	pcur = cmark_p;

#if 0
	if (pcur->pnext == pcur) {
		has_only_one_block = 1;
	}
#endif

	/* check finded block */
	if ( pcur->psize >= psize ) {
		/* change list and return value */
		if (pcur->psize > psize ) {
			pcur->psize -= psize;
			tmp = cmark_p->pnext;
			cmark_p->pprev->pnext = pcur + PAGE_SIZE * psize;
			tmp->pprev = pcur + PAGE_SIZE * psize;
			cmark_p = copy_mark( pcur , pcur + PAGE_SIZE * psize );
			return pcur;
		} else {
			/* psize == pcur->psize */
			pcur->pprev->pnext = pcur->pnext;
			pcur->pnext->pprev = pcur->pprev;
			cmark_p = pcur->pnext;
			return pcur;
		}
	}

	return NULL;
}

/* free page that was allocated */
void page_free(pmark_t *paddr) { /* Don't work!!!! */
	/* find */
	paddr->psize = 1;
	paddr->pnext = cmark_p->pnext;
	paddr->pprev = cmark_p;
	cmark_p->pnext->pprev = paddr;
	cmark_p->pnext = paddr;
}



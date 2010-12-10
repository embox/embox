/**
 * @file
 * @brief page allocator
 * @details Use `page allocator' when we need allocate or free only one page,
 * else see `multipage allocator' or `malloc'.
 *
 * @date 04.04.10
 * @author Fedor Burdun
 */

#include <errno.h>
#include <kernel/mm/opallocator.h>

/** Structure of page marker. It occupies at the begin of each free memory
 * block
 * psize - count of page
 * pnext, pprev - pointers other free blocks
 */
#ifndef EXTENDED_TEST
typedef struct pmark {
	size_t psize;
	struct pmark *pnext;
	struct pmark *pprev;
}pmark_t;

extern char _heap_start;
extern char _heap_end;
#endif

// CONFIG_PAGE_SIZE

#ifndef PAGE_QUANTITY
#define PAGE_QUANTITY (((size_t)(&_heap_end - &_heap_start) ) / CONFIG_PAGE_SIZE)
#endif

int page_alloc_hasinit = 0;

#ifdef EXTENDED_TEST

static uint8_t page_pool[PAGE_QUANTITY][CONFIG_PAGE_SIZE];
static pmark_t *cmark_p = (pmark_t *) page_pool;
#else
//#define START_MEMORY_ADDR 0x40000000
static pmark_t *cmark_p = (pmark_t *) &_heap_start;
#endif

#ifdef EXTENDED_TEST
pmark_t* get_cmark_p(void) {
	return cmark_p;
}
#endif

#if 0
/* defragmentation pages */
static void page_defrag(void) {
}
#endif

/* copy mark structure */
static pmark_t *copy_mark(pmark_t *from, pmark_t *to) {
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
void *opalloc(void) {
	/* size_t psize = 1; */
	pmark_t *pcur, *tmp, *tt;

	if (!page_alloc_hasinit) {
		page_alloc_init();
		page_alloc_hasinit = 1;
	}

	if (cmark_p == NULL) { /* don't exist memory enough */
		/* generate error */
		#if 0
		errno = ENOMEM;
		#endif
		return NULL;
	}
	/* find first proper block */
	pcur = cmark_p;

	#if 0
	printf("cmark_p: %d size: %d \n",cmark_p,cmark_p->psize);
	#endif

	/* check finded block */

	/* change list and return value */
	if (pcur->psize > 1) { /* 1 := psize */
		tt = (pmark_t *) ((unsigned long) pcur +
			(unsigned long) CONFIG_PAGE_SIZE * (unsigned long) 1);
		/* 1:= psize */
		pcur->psize -= 1; /* 1 := psize */
		tmp = cmark_p->pnext;
		cmark_p->pprev->pnext = tt;
		tmp->pprev = tt;
		cmark_p = copy_mark(pcur, tt);
		return pcur;
	} else {/* psize =: 1 == pcur->psize */
		if (pcur->pnext == pcur) { /* it's last block */
			cmark_p = NULL;
			return pcur;
		} else {
			pcur->pprev->pnext = pcur->pnext;
			pcur->pnext->pprev = pcur->pprev;
			cmark_p = pcur->pnext;
			return pcur;
		}
	}
	return NULL;
}

/* free page that was allocated */
void opfree(void *addr) {
	pmark_t *paddr = (pmark_t*) addr;
	#if 0
	if (paddr == NULL) {
		printf("PAGE FREE: try free NULL pointer!!!\n");
		return;
	}
	#endif

	paddr->psize = 1;

	if (cmark_p == NULL) { /* if don't exist any free page */
		paddr->pnext = paddr;
		paddr->pprev = paddr;
		cmark_p = paddr;
	}

	paddr->pnext = cmark_p->pnext;
	paddr->pprev = cmark_p;
	cmark_p->pnext->pprev = paddr;
	cmark_p->pnext = paddr;
}



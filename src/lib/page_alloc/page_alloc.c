/**
 * @file
 * @brief page allocator
 * @details
 *
 * @date 04.04.10
 * @author Fedor Burdun
 */

#if 0 /* Example code for use from header file */
		typedef long psize_t;
		typedef long paddr_t;
		const psize_t MAXPAGECOUNT 	= 0xffff;
		const paddr_t SIZEOFPAGE	= 0xff;
		pmark_t *cmark_p;
		void *mem;

#ifdef DEBUG_x86_ONLY /* Code included for debug specific */
#else /* Embox specific code */
#endif /* End of specific code */

/* structure of page marker */
struct pmark_t {
	psize_t	psize;
	pmark_t *pnext;
};

#endif

/* copy mark structure */
paddr_t copy_mark( paddr_t from , paddr_t to ) {
	to->psize = from->psize;
	to->pnext = from->pnext;
	to->pprev = from->pprev;
	return to;
}

/* Initialize page allocator */
void page_alloc_init() {
#ifdef DEBUG_x86_ONLY /* Code included for debug specific */
	mem = malloc( MAXPAGECOUNT * SIZEOFPAGE );
#else /* Embox specific code */
#endif /* End of specific code */
	cmark_p = mem;
	cmark_p->psize = MAXPAGECOUNT;
	cmark_p->pnext	= cmark_p;
	palloc_hasinit	= 1;
}

/* allocate page */
paddr_t page_alloc(psize_t psize) {
	if (!palloc_hasinit) {
		page_alloc_init();
	}
	/* find first proper block */
	struct pmark_t *pcur;
	pcur = cmark_p->next;
	while ( pcur != cmark_p && pcur->psize < psize ) {
		pcur = pcur->next;
	}
	/* check finded block */
	if ( pcur->psize >= psize ) {
		/* change list and return value */
		if (pcur->psize > psize ) {
			pcur->psize = pcur->psize - psize;
			cmark_p = copy_mark( pcur , pcur + SIZEOFPAGE * psize );
			return pcur;
		} else {
			/* psize == pcur->psize */
			pcur->pprev->next = pcur->next;
			return pcur;
		}
	} else {
		#if 0
		page_defrag();
		return page_alloc(psize);
		#endif
		/* generate memory error */
	}
}

/* free page that was allocated */
void page_free(paddr_t paddr) {
	/* find */
}

/* defragmentation pages */
void page_defrag() {
}


/**
 * @file
 * @brief multipage allocator buddy algorithm
 * @details Use `page allocator' when we need allocate or free only one page,
 * else see `multipage allocator' or `malloc'.
 *
 * @date 01.07.10
 * @author Fedor Burdun
 */

/**
 * `taddr' is index in binary tree
 * left child of node with index `addr' is addr*2
 * right child -/-/- is addr*2+1
 * addr of root is 1
 *
 * BITS:
 *  1 - left subtree partially released
 *  2 - right subtree partially released
 *  4 - node is released or released all of chlidren
 *  8 - node isn't in heap
 */

/* MY code MARKS
 * TFMBR - this function must be rewriting
 * TFwR  - this function was rewriting
 * TFW   - this function work (while don't see any bugs)
 */

#include <errno.h>
#include <kernel/mm/mpallocator.h>

#ifndef EXTENDED_TEST
extern char _heap_start;
extern char _heap_end;
# define HEAP_START_PTR 	(&_heap_start)
# define HEAP_END_PTR		(&_heap_end)
#else
extern char *_heap_start;
extern char *_heap_end;
# define HEAP_START_PTR 	_heap_start
# define HEAP_END_PTR		_heap_end
#endif

#define PAGE_QUANTITY ( (((size_t) HEAP_END_PTR - (size_t) HEAP_START_PTR) ) / CONFIG_PAGE_SIZE )

#define SET_BIT1(ind,bit) set_bits( ind , get_bits(ind) | (bit) )
#define SET_BIT0(ind,bit) set_bits( ind , get_bits(ind) & (255^(bit)) )
#define HAS_BIT(ind,bit) ( get_bits(ind) & (bit) )

#define ANY_OPTIMIZATION


typedef size_t taddr;	/* addres in tree */
char* heap_start;		/* real heap_start */
size_t sizeofpool;		/* real size of heap */
int hasinit = 0;
size_t maxblocksize;	/* size of real max block (may be not root) */
size_t rootblocksize;	/* size of root block */

/**
 * bit pack
 * no real bit pack. only array mapping
 * addr - 1 because root is 1
 */
inline void set_bits( taddr addr , char bit ) {
	(HEAP_START_PTR) [addr-1] = bit;
}

inline char get_bits( taddr addr ) {
	return (HEAP_START_PTR) [addr-1];
}

/**		TFW
 * initialization of tree
 * Mark block if it isn't in heap.
 *
 * return 1 if subtree isn't in heap
 * return 0 if node with subtree is in heap
 */
int dfs_init( taddr addr , char * ptr , size_t size ) {
	int marked = 0;

	if ( (ptr + ( size * CONFIG_PAGE_SIZE ) ) <= HEAP_END_PTR ) {
		/* block is in heap */
		set_bits( addr , 0 );
	} else {
		/* block isn't in heap */
		set_bits( addr , 8 );
		marked = 1;
	}

	/* if it's no leaf */
	if ( size > 1 ) {
		/* initialize left subtree */
		if ( dfs_init( 2*addr , ptr , size/2 ) ) {
			SET_BIT1( addr , 1 );
			marked = 1;
		}
		/* initialize right subtree */
		if ( dfs_init( 2*addr+1 , ptr + CONFIG_PAGE_SIZE*size/2 , size/2 ) ) {
			SET_BIT1( addr , 2 );
			marked = 1;
		}
	}
	return marked;
}

#if 0 																//NOT USED CODE

/**		TFMBR!!!
 * Check addr in tree or not.
 */
int taddr_in_tree( taddr addr ) {
	size_t len = 1;
	for ( ; addr < maxblocksize ; addr <<= 1 )
		len <<= 1;
	return len+addr <= sizeofpool; // ?!?!? <= or <
}
#endif

/**		TFW
 * initialization of allocator
 */
#ifdef EXTENDED_TEST
	extern
#endif
void multipage_init(void) {
	int sizeoftree;
	int qpt;

	int page_quantity = PAGE_QUANTITY;
	/* calculate maxblocksize */
	for ( rootblocksize=1 ; rootblocksize < page_quantity ; rootblocksize*=2 );
	sizeoftree = 2*rootblocksize; /* one byte for one page (no bit pack) */

	/* (sizeoftree - 1) / 0x1000 + 1 // quantity of pages for tree */
	qpt = ( (sizeoftree - 1) / CONFIG_PAGE_SIZE + 1 );

	heap_start = (HEAP_START_PTR) + CONFIG_PAGE_SIZE * qpt;
	sizeofpool = PAGE_QUANTITY - qpt;

	maxblocksize = rootblocksize == sizeofpool ? rootblocksize : rootblocksize / 2;

	dfs_init( 1 , heap_start , rootblocksize );
}

/**
 * convert place in tree to pointer
 */
void * taddr_to_ptr( taddr addr ) {
	//taddr saddr = addr;
	for ( ; addr<rootblocksize ; addr*=2 ) ;
	return heap_start + (addr-rootblocksize)*CONFIG_PAGE_SIZE;
}

/**
 * check block for alloc
 */
inline int is_avail( taddr addr ) {
	return !get_bits( addr );
}

/**
 * check block in heap or not
 */
inline int in_heap( taddr addr, size_t length ) {
	return ( (unsigned long long) taddr_to_ptr( addr ) +
		length*CONFIG_PAGE_SIZE <= (unsigned long long) _heap_end );
}

/**		TFwR
 * find first available proper block for reserve.
 * return 0 if same block don't exist.
 */
taddr dfs_find( taddr lroot, size_t cursize, size_t size ) {
	/* must check above that size != 0 */
	/* enough codition ( it's reserved or no proper )
		(if lroot not belongs main tree, than cursize == 0) */
#ifdef EXTENDED_TEST_
	printf("\n\n TESTDFS  %d %d %d\n", lroot , get_bits(lroot), HAS_BIT( lroot , 4 ));
#endif
	if ( cursize < size || HAS_BIT( lroot , 4 ) ) return 0;
	taddr child_return;
	/* find in left subtree */
	if (child_return = dfs_find( lroot*2 , cursize/2 , size ))
		return child_return;
	/* find in right subtree */
	if (child_return = dfs_find( lroot*2+1 , cursize/2 , size ))
		return child_return;
	/* may be it is current block*/
	if ( is_avail( lroot ) && in_heap( lroot, cursize) ) return lroot;
	/* no exist proper block */
	return 0;
}

/** 	TFwR
 * allocator
 */
void * mpalloc( size_t size ) {
	size_t size_fr; /* for return */
	taddr block,parent,taddr_fr;

	/* initialization of allocator */
    if (!hasinit) {
        multipage_init();
        hasinit = 1;
    }

	/* find proper block */
	if ( !( block = dfs_find( /* root of tree */ 1 , rootblocksize , size ) ) )
		return NULL; /* no exist proper block */

	set_bits(block,4);
	taddr_fr = block;
	for (; block > 1 ; block = parent ) {
		SET_BIT1( parent = block >> 1 , block & 1 ? 2 : 1 );
			/* mark from left or right child for node */
		#ifdef ANY_OPTIMIZATION
		/* if children was released then parent released */
		if ( HAS_BIT(block, 4) && HAS_BIT(block^1, 4) ) {
			SET_BIT1( parent , 4 );
		}
		#endif
	}
	return taddr_to_ptr(taddr_fr);
}

/**		TFMBR
 * free block, that was allocated
 */
void mpfree( void * ptr ) {
#ifdef EXTENDED_TEST
	printf("run free\n");
	if ( ptr == NULL ) return;
#endif
	taddr parent,before,addr;
	addr = ((size_t) ptr - (size_t) heap_start)/CONFIG_PAGE_SIZE;
	addr = rootblocksize + ((size_t) ptr - (size_t) heap_start)/CONFIG_PAGE_SIZE;
	/* in OLD version was different and WORK!!! */
	for ( before=0 ; (addr>0) && (!get_bits(addr)) && !(before & 1) ; addr = (before = addr) >> 1 );
	/* addr was allocated */
	if (get_bits(addr)==4) { /* else if don't equal 0 algorithm has ERROR!!! */
		#ifdef ANY_OPTIMIZATION
		/* now exist alow block */
		for (parent=addr ; parent >= 1 ; parent >>= 1 ) {
			SET_BIT0(parent, 4);
		}
		#else
		set_bits(addr, 0);
		#endif
		/* it code wrong! */
		for (; addr > 1 ;) {
			/* expected that bits[addr] == 0 */
			parent = addr >> 1;
			SET_BIT0(parent,addr & 1 ? 2 : 1); /* unmark from left or right child for node */
			/* if unmarked left or right node then unmark parent (needed mark) */
			/* if (!(get_bits(parent) | get_bits(parent^1))) { */
			if (!get_bits(parent)) {
				addr = parent;
			} else {
				break;
			}
			addr = parent;
		}
	}
}

/**
 * some functions for debug
 */
//#ifdef EXTENDED_TEST
#if 1

#if 0
# define MULTIPAGE_TRACE printk
#else
# define MULTIPAGE_TRACE printf
#endif

extern void multipage_info() {
	char *ptr;
	MULTIPAGE_TRACE("multipage_alloc info\n\tPAGE_QUANTITY=(hex)%16x\n",PAGE_QUANTITY);
	MULTIPAGE_TRACE("\tCONFIG_PAGE_SIZE=(hex)%08x\n",CONFIG_PAGE_SIZE);
	MULTIPAGE_TRACE("\tPAGE_QUANTITY=(dec)%ld\n",PAGE_QUANTITY);
	MULTIPAGE_TRACE("\tpool start: %08x \n\tpool end: %08x \n",(unsigned long) HEAP_START_PTR,
		(unsigned long) HEAP_END_PTR);
	MULTIPAGE_TRACE("\treal heap start (for return): %08x\n",(unsigned long) heap_start);
	MULTIPAGE_TRACE("\tmaxblocksize=(dec)%ld\n",maxblocksize);
	MULTIPAGE_TRACE("\tsize of pool(real)=(hex)%08x\n",sizeofpool);
	MULTIPAGE_TRACE("\tsize of pool(real)=(dec)%ld\n",sizeofpool);
	MULTIPAGE_TRACE("\n\tTree:\n\t\t");
	for ( ptr=HEAP_START_PTR ; ptr<(_heap_start+2*rootblocksize) ; ++ptr ) {
		MULTIPAGE_TRACE("%ld ",*ptr);
	}
	MULTIPAGE_TRACE("\n");
	MULTIPAGE_TRACE("info end\n");
}

#undef MULTIPAGE_TRACE

#endif

/**
 * @file
 * @brief multipage allocator buddy algorithm
 * @details Use `page allocator' when we need allocate or free only one page,
 * else see `multipage allocator' or `malloc'.
 *
 * @date 01.07.10
 * @author Fedor Burdun
 */

/* MY MARKS
 * TFMBR - this function must be rewriting
 */

#include <errno.h>
#include <lib/multipage_alloc.h>

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

#define SET_BIT1(ind,bit) set_bits( ind , get_bits(ind) | 1 )
#define SET_BIT0(ind,bit) set_bits( ind , get_bits(ind) & (255^bit) )
#define HAS_BIT(ind,bit) ( get_bits(ind) | bit )

typedef size_t taddr;		/* addres in tree */
char* heap_start;		/* real heap_start */
size_t sizeofpool;			/* real size of heap */
int hasinit = 0;
size_t maxblocksize;

/**
 * bit pack
 * no real bit pack. only array mapping
 */
void set_bits( taddr addr , char bit ) {
	(HEAP_START_PTR) [addr] = bit;
}

char get_bits( taddr addr ) {
	return (HEAP_START_PTR) [addr];
}

/**		TFMBR!!!!
 * initialization of tree
 * Mark block if it isn't in heap.
 */
void rec_init( taddr addr , char * ptr , size_t size ) {
	if ( (ptr + ( size * CONFIG_PAGE_SIZE ) ) <= HEAP_END_PTR ) {

		#ifdef EXTENDED_TEST_
			printf("rec_init: \n\taddr:(dec) %ld\n\tbegin:(hex) \t%08x\n\tend:(hex) \t%08x",
				addr , ptr , (ptr+(size*CONFIG_PAGE_SIZE)) );
			printf("rec_init: set bit 0 on taddr: %ld \n",addr);
		#endif

		set_bits( addr , 0 );
	} else {

		#ifdef EXTENDED_TEST_
			printf("rec_init: \n\taddr:(dec) %ld\n\tbegin:(hex) \t%08x\n\tend:(hex) \t%08x",
				addr , ptr , (ptr+(size*CONFIG_PAGE_SIZE)) );
			printf("rec_init: set bit 1 on taddr: %ld \n",addr);
		#endif

		set_bits( addr , 1 );
	}
	if ( size > 1 ) {
		rec_init( 2*addr , ptr , size/2 );
		rec_init( 2*addr+1 , ptr + CONFIG_PAGE_SIZE*size/2 , size/2 );
	}
}

/**		TFMBR!!!
 * Check addr in tree or not.
 */
int taddr_in_tree( taddr addr ) {
	size_t len = 1;
	for ( ; addr < maxblocksize ; addr <<= 1 )
		len <<= 1;
	return len+addr <= sizeofpool; // ?!?!? <= or <
}

/**		TFMBR!!!
 * initialization of allocator
 */
#ifdef EXTENDED_TEST
	extern
#endif
void multipage_init(void) {
	int sizeoftree;
	int qpt;

	/* calculate maxblocksize */
	for ( maxblocksize=1 ; maxblocksize<PAGE_QUANTITY ; maxblocksize*=2 );
	sizeoftree = 2*maxblocksize / 1; /* change 1 to 8 (bit pack) */
	// sizeoftree = maxblocksize / 4;

	/* (sizeoftree - 1) / 0x1000 + 1 // quantity of pages for tree */
	qpt = ((sizeoftree - 1)/CONFIG_PAGE_SIZE+1);

	heap_start = (HEAP_START_PTR) + CONFIG_PAGE_SIZE * qpt;
	sizeofpool = PAGE_QUANTITY - qpt;

	rec_init( 1 , heap_start , maxblocksize );
}

/**
 * convert place in tree to pointer
 */
void * taddr_to_ptr( taddr addr ) {
	//taddr saddr = addr;
	for ( ; addr<maxblocksize ; addr*=2 ) ;
	return heap_start + (addr-maxblocksize)*CONFIG_PAGE_SIZE;
}

/**
 * free block by taddr
 */
void free_addr( taddr addr ) {
	if (get_bits(addr)) {
		set_bits( addr , 0 );
	}

	if (!get_bits(addr^1)) {
		free_addr( addr/2 );
	}
}

void free_down( taddr addr ) {
	if (addr>=(maxblocksize<<1)) {
		return;
	}

	set_bits( addr , 0 );

	free_down( addr<<1 );
	free_down( (addr<<1)+1 );
}

/**
 * mark blocks
 */
void mark_blocks( taddr block ) {
	if (block>=2*maxblocksize) return;
	set_bits(block,1);
	mark_blocks(block*2);
	mark_blocks(block*2+1);
}

/**
 * check, block is available or not
 */
int avail( taddr block ) {
	if (block>=maxblocksize) {
		return !get_bits(block) && (block-maxblocksize < sizeofpool);
	}
	return !get_bits(block) && avail(block*2) && avail(block*2+1);
}

/**
 * check, block is marked or not
 */
int marked( taddr block ) {
	if (block>=maxblocksize) {
		return get_bits(block) && (block-maxblocksize < sizeofpool);
	}
	return get_bits(block) && marked(block*2) && marked(block*2+1);
}

/**
 * find first proper block
 * addr is addres of first block needed size
 */
void * find_block_avail( taddr addr ) {
	taddr i;
	for ( i=0 ; (i<addr)&&(!avail(addr+i)) ; ++i ); // better if 'for' will be from size to 0
	if ( i < addr ) {
		mark_blocks(addr+i);
		return taddr_to_ptr(addr+i);
	} else {
		return NULL;
	}
}

/**
 * check block for alloc
 */
inline int is_avail(taddr addr) {
	return !get_bits(addr);
}

/**
 * check block in heap or not
 */
inline int in_heap(taddr addr,size_t length) {
#ifdef EXTENDED_TEST_no
	printf("<< %d len %d\n",addr,length);
#endif
	return ( (long long) taddr_to_ptr(addr) + length*CONFIG_PAGE_SIZE <= (long long) _heap_end );
}

/**
 * profit
 * return NULL if has no block
 */
taddr find_by_size( size_t size ) {
	taddr i;
	taddr start = maxblocksize/size; /* start index */ // ?!?!?!? no profit!!!!! maxblocksize +/- ^2
	for ( i=0; (i<start)&&(!is_avail(i+start)); ++i );
	if ( in_heap(start+i,size) )
		return start+i;
	else
		return NULL;
}

/**
 * allocator
 */
void * multipage_alloc( size_t size ) {
	size_t size_fr; /* for return */
	taddr block,parent,taddr_fr;
	/* block is proper and empty */
	for ( size_fr=1 ; (size_fr < maxblocksize) &&
		( NULL != (block = find_by_size(size_fr)) ) ; size_fr<<=1 );
	set_bits(block,1|2);
	taddr_fr = block;
	for (; block >= 1 ; ) {
		parent = block >> 1;
		SET_BIT1(parent,block & 1 ? 2 : 1); /* unmark from left or right child for node */
		block = parent;
	}
	return taddr_to_ptr(taddr_fr);
}

/**
 * find block, that was allocated
 * 			DON"T WORK DON"T WORK AAAAAAA!!!!!!!!!!!
 */
void robin_taddr( void * ptr ) {
	taddr before;
	taddr saddr = ((size_t) ptr - (size_t) heap_start)/CONFIG_PAGE_SIZE;
	saddr += maxblocksize;

	for ( ; !(saddr & 1) && marked(saddr) ; saddr=(before=saddr)>>1 );  // ?!?!?!?! DON"T UNDERSTAND IT

	if (marked(saddr)) {
		free_addr( saddr );
		free_down( saddr );
		return;
	}

	if (marked(before)) {
		free_addr( before );
		free_down( before );
		return;
	}
}

/**
 * free block, that was allocated
 */
void multipage_free( void * ptr ) {
	taddr parent,before;
	taddr addr = ((size_t) ptr - (size_t) heap_start)/CONFIG_PAGE_SIZE;
	addr += maxblocksize; /* must be size of root element (==?!?!) */
	/* in OLD version was different and WORK!!! */
	for (; (addr>0)&&(!get_bits(addr))&& !(before & 1) ; addr = (before = addr) >> 1 );
	/* addr was allocated */
	if (get_bits(addr)==3) {
		set_bits(addr,0);
		for (; addr > 1 ;) {
			/* expected that bits[addr] == 0 */
			parent = addr/2;
			SET_BIT0(parent,addr & 1 ? 2 : 1); /* unmark from left or right child for node */
			/* if unmarked left and right node then unmark parent */
			if (!(get_bits(parent) | get_bits(parent^1))) {
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
#ifdef EXTENDED_TEST

extern void multipage_info() {
	char *ptr;
	printf("multipage_alloc info\n\tPAGE_QUANTITY=(hex)%16x\n",PAGE_QUANTITY);
	printf("\tCONFIG_PAGE_SIZE=(hex)%08x\n",CONFIG_PAGE_SIZE);
	printf("\tPAGE_QUANTITY=(dec)%ld\n",PAGE_QUANTITY);
	printf("\tpool start: %08x \n\tpool end: %08x \n",(unsigned long) HEAP_START_PTR,
		(unsigned long) HEAP_END_PTR);
	printf("\treal heap start (for return): %08x\n",(unsigned long) heap_start);
	printf("\tmaxblocksize=(dec)%ld\n",maxblocksize);
	printf("\tsize of pool(real)=(hex)%08x\n",sizeofpool);
	printf("\tsize of pool(real)=(dec)%ld\n",sizeofpool);
	printf("\n\tTree:\n\t\t");
	for ( ptr=HEAP_START_PTR ; ptr<heap_start ; ++ptr ) {
		printf("%ld ",*ptr);
	}
	printf("\n");
	printf("info end\n");
}

#endif

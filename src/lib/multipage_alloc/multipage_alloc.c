/**
 * @file
 * @brief multipage allocator
 * @details Use `page allocator' when we need allocate or free only one page,
 * else see `multipage allocator' or `malloc'.
 *
 * @date 28.04.10
 * @author Fedor Burdun
 */

#include <errno.h>
#include <lib/multipage_alloc.h>

extern char _heap_start;
extern char _heap_end;

#define PAGE_QUANTITY ( ((size_t) (&_heap_end - &_heap_start) ) / CONFIG_PAGE_SIZE )

typedef int taddr;		/* addres in tree */
char* heap_start;		/* real heap_start */
int sizeofpool;			/* real size of heap */
int hasinit = 0;
size_t maxblocksize;

void set_bit( taddr addr , char bit ) {
	(&_heap_start) [addr] = bit;
}

char get_bit( taddr addr ) {
	return (&_heap_start) [addr];
}

#if 0
void set_bit( taddr addr , char bit ) {
	//char tail = addr && 7;
	//addr >>= 3;
	if (bit) {
		(&_heap_start)[addr >> 3] |= 1 << (addr & 7);
	} else {
		(&_heap_start)[addr >> 3] &= 255 ^ (1 << (addr & 7));
	}
}

char get_bit( taddr addr ) {
	return ( (&_heap_start)[addr >> 3] & (1 << (addr & 7)) ) ? 1 : 0 ;
}
#endif

void rec_init( taddr addr , char * ptr , size_t size ) {
	if ( (ptr + ( size * CONFIG_PAGE_SIZE ) ) <= &_heap_end )
		set_bit( addr , 1 );
	else
		set_bit( addr , 0 );
	if ( size > 1 ) {
		rec_init( 2*addr , ptr , size/2 );
		rec_init( 2*addr+1 , ptr + size/2 , size/2 );
	}
}

void multipage_init() {
	int sizeoftree  = PAGE_QUANTITY / 1; /* change 1 to 8 (bit pack) */
	/* (sizeoftree - 1) / 0x1000 + 1 // quantity of pages of tree */
	int qpt = ((sizeoftree - 1)/CONFIG_PAGE_SIZE+1);
	heap_start = (&_heap_start) + CONFIG_PAGE_SIZE * qpt;
	sizeofpool = PAGE_QUANTITY - qpt;

	for ( maxblocksize=1 ; maxblocksize<sizeofpool ; maxblocksize*=2 );
	rec_init( 1 , heap_start , maxblocksize );
}

void * taddr_to_ptr( taddr addr ) {
	taddr saddr = addr;
	for ( ; addr<sizeofpool ; addr*=2 ) ;
	return heap_start + (saddr-sizeofpool)*CONFIG_PAGE_SIZE;
}

void free_addr( size_t addr ) {
	if (get_bit(addr) && !get_bit(addr^1)) {
		set_bit( addr , 0 );
		free_addr( addr/2 );
	}
}

void robin_taddr( void * ptr ) {
	taddr saddr = ((size_t) ptr - (size_t) &_heap_start)/CONFIG_PAGE_SIZE;
	saddr += sizeofpool;
	for ( ; !(saddr & 1) && get_bit(saddr) ; saddr>>=1 );
	free_addr( saddr );
}

void * find_block_avail( size_t size ) {
	size_t i;
	for ( i=0 ; (i<size)&&(!get_bit(size+i)) ; ++i );
	if ( i < size ) {
		return taddr_to_ptr(size+i);
	} else {
		return NULL;
	}
}

void * multipage_alloc( size_t size ) {
	size_t size_fr;
	void * ptr = NULL;
	if (!hasinit) {
		multipage_init();
		hasinit = 1;
	}
	for ( size_fr=1 ; ( size_fr < size ) ; size_fr*=2 );
	for ( ; (size_fr <= maxblocksize) && !(ptr=find_block_avail(size_fr)) ; size_fr*=2 );
	return ptr;
}

void multipage_free( void * ptr ) {
	robin_taddr( ptr );
}


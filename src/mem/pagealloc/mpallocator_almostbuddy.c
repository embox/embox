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
#include <mem/pagealloc/mpallocator.h>

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

#define PAGE_QUANTITY ((((size_t)HEAP_END_PTR - \
			(size_t)HEAP_START_PTR)) / CONFIG_PAGE_SIZE)

typedef size_t taddr; /* addres in tree */
char* heap_start;     /* real heap_start */
size_t sizeofpool;    /* real size of heap */
int hasinit = 0;
size_t maxblocksize;

/**
 * bit pack
 */
void set_bits(taddr addr, char bit) {
	(HEAP_START_PTR)[addr] = bit;
}

char get_bits(taddr addr) {
	return (HEAP_START_PTR)[addr];
}

#if 0
void set_bits(taddr addr, char bit) {
	//char tail = addr && 7;
	//addr >>= 3;
	if (bit) {
		(HEAP_START_PTR)[addr >> 3] |= 1 << (addr & 7);
	} else {
		(HEAP_START_PTR)[addr >> 3] &= 255 ^ (1 << (addr & 7));
	}
}

char get_bits(taddr addr) {
	return ((HEAP_START_PTR)[addr >> 3] & (1 << (addr & 7))) ? 1 : 0;
}
#endif

/**
 * initialization of tree
 * Mark block if it isn't in heap.
 */
void rec_init(taddr addr, char *ptr, size_t size) {
	if ((ptr + (size * CONFIG_PAGE_SIZE)) <= HEAP_END_PTR) {

		#ifdef EXTENDED_TEST_
		printf("rec_init: \n\taddr:(dec) %ld\n\tbegin:(hex) \t%08x\n\tend:(hex) \t%08x",
			addr, ptr, (ptr + (size * CONFIG_PAGE_SIZE)));
		printf("rec_init: set bit 0 on taddr: %ld \n", addr);
		#endif

		set_bits(addr, 0);
	} else {

		#ifdef EXTENDED_TEST_
		printf("rec_init: \n\taddr:(dec) %ld\n\tbegin:(hex) \t%08x\n\tend:(hex) \t%08x",
			addr, ptr, (ptr + (size * CONFIG_PAGE_SIZE)));
		printf("rec_init: set bit 1 on taddr: %ld \n", addr);
		#endif

		set_bits(addr, 1);
	}
	if (size > 1) {
		rec_init(2 * addr, ptr, size / 2);
		rec_init(2 * addr + 1,
			ptr + CONFIG_PAGE_SIZE * size / 2, size / 2);
	}
}

/**
 * Check addr in tree or not.
 */
int taddr_in_tree(taddr addr) {
	size_t len = 1;
	for ( ; addr < maxblocksize; addr <<= 1) {
		len <<= 1;
	}
	return len + addr <= sizeofpool; // ?!?!? <= or <
}

/**
 * initialization of allocator
 */
#ifdef EXTENDED_TEST
	extern
#endif
void multipage_init(void) {
	int sizeoftree;
	int qpt;

	/* calculate maxblocksize */
	for (maxblocksize = 1; maxblocksize < PAGE_QUANTITY; maxblocksize *= 2);
	sizeoftree = 2 * maxblocksize / 1; /* change 1 to 8 (bit pack) */
	// sizeoftree = maxblocksize / 4;

	/* (sizeoftree - 1) / 0x1000 + 1 // quantity of pages for tree */
	qpt = ((sizeoftree - 1) / CONFIG_PAGE_SIZE + 1);

	heap_start = (HEAP_START_PTR) + CONFIG_PAGE_SIZE * qpt;
	sizeofpool = PAGE_QUANTITY - qpt;

	rec_init(1, heap_start, maxblocksize);
}

/**
 * convert place in tree to pointer
 */
void *taddr_to_ptr(taddr addr) {
	//taddr saddr = addr;
	for ( ; addr < maxblocksize; addr *= 2);
	return heap_start + (addr - maxblocksize) * CONFIG_PAGE_SIZE;
}

/**
 * free block by taddr
 */
void free_addr(taddr addr) {
	if (get_bits(addr)) {
		set_bits(addr, 0);
	}

	if (!get_bits(addr^1)) {
		free_addr(addr / 2);
	}
}

void free_down(taddr addr) {
	if (addr >= (maxblocksize << 1)) {
		return;
	}

	set_bits(addr, 0);

	free_down(addr << 1);
	free_down((addr << 1) + 1);
}

/**
 * mark blocks
 */
void mark_blocks(taddr block) {
	if (block >= 2 * maxblocksize) {
		return;
	}
	set_bits(block, 1);
	mark_blocks(block * 2);
	mark_blocks(block * 2 + 1);
}

/**
 * check, block is available or not
 */
int avail(taddr block) {
	if (block >= maxblocksize) {
		return !get_bits(block) && (block - maxblocksize < sizeofpool);
	}
	return !get_bits(block) && avail(block * 2) && avail(block * 2 + 1);
}

/**
 * check, block is marked or not
 */
int marked(taddr block) {
	if (block >= maxblocksize) {
		return get_bits(block) && (block - maxblocksize < sizeofpool);
	}
	return get_bits(block) && marked(block * 2) && marked(block * 2 + 1);
}

/**
 * find first proper block
 * addr is addres of first block needed size
 */
void *find_block_avail(taddr addr) {
	taddr i;
	// better if 'for' will be from size to 0
	for (i = 0; (i < addr) && (!avail(addr + i)); ++i);
	if (i < addr) {
		mark_blocks(addr + i);
		return taddr_to_ptr(addr + i);
	} else {
		return NULL;
	}
}

/**
 * allocator
 */
void *mpalloc(size_t size) {
	size_t size_fr;
	void * ptr = NULL;
	if (!hasinit) {
		multipage_init();
		hasinit = 1;
	}
	for (size_fr = 1; (size_fr < size); size_fr <<= 1);
	for (; (size_fr <= maxblocksize) &&
	    !(ptr=find_block_avail(maxblocksize/size_fr)); size_fr <<= 1); /* WHY maxblocksize/size_fr ?? */
	/* may be enought only ptr=find_block_avail(size_fr)
	   because if size_fr isn't available, than size_fr*2 also isn't available */
	return ptr;
}

/**
 * find block, that was allocated
 */
void robin_taddr(void *ptr) {
	taddr before = 0; /* for no warinings */
	taddr saddr = ((size_t)ptr - (size_t)heap_start) / CONFIG_PAGE_SIZE;
	saddr += maxblocksize;

	for ( ; !(saddr & 1) && marked(saddr); saddr = (before = saddr) >> 1);

	if (marked(saddr)) {
		free_addr(saddr);
		free_down(saddr);
		return;
	}

	if (marked(before)) {
		free_addr(before);
		free_down(before);
		return;
	}
}

/**
 * free block, that was allocated
 */
void mpfree(void *ptr) {
	if (ptr == NULL) {
		/* errno = XXX */
		return;
	}
	robin_taddr(ptr);
}

/**
 * return list of free and busy blocks in heap
 */
void mpget_blocks_info(struct list_head* list) {
	//TODO:
}

/**
 * some functions for debug
 */
#ifdef EXTENDED_TEST

extern void multipage_info(void) {
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
	for (ptr = HEAP_START_PTR; ptr < heap_start; ++ptr) {
		printf("%ld ", *ptr);
	}
	printf("\n");
	printf("info end\n");
}

#endif

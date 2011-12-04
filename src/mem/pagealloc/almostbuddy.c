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
#include <mem/page.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(page_alloc_init);

extern char *_heap_start;
extern char *_heap_end;
# define HEAP_START_PTR 	_heap_start
# define HEAP_END_PTR		_heap_end


#define PAGE_QUANTITY ((((size_t)HEAP_END_PTR - \
			(size_t)HEAP_START_PTR)) / CONFIG_PAGE_SIZE)

typedef size_t taddr; /* addres in tree */
static char* heap_start;     /* real heap_start */
static size_t sizeofpool;    /* real size of heap */
//int hasinit = 0;
static size_t maxblocksize;

/**
 * bit pack
 */
static void set_bits(taddr addr, char bit) {
	(HEAP_START_PTR)[addr] = bit;
}

static char get_bits(taddr addr) {
	return (HEAP_START_PTR)[addr];
}


/**
 * initialization of tree
 * Mark block if it isn't in heap.
 */
static void rec_init(taddr addr, char *ptr, size_t size) {
	if ((ptr + (size * CONFIG_PAGE_SIZE)) <= HEAP_END_PTR) {
		set_bits(addr, 0);
	} else {
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
//static int taddr_in_tree(taddr addr) {
//	size_t len = 1;
//	for ( ; addr < maxblocksize; addr <<= 1) {
//		len <<= 1;
//	}
//	return len + addr <= sizeofpool; // ?!?!? <= or <
//}

/**
 * initialization of allocator
 */
static int page_alloc_init(void) {
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
	return 0;
}

/**
 * convert place in tree to pointer
 */
static void *taddr_to_ptr(taddr addr) {
	//taddr saddr = addr;
	for ( ; addr < maxblocksize; addr *= 2);
	return heap_start + (addr - maxblocksize) * CONFIG_PAGE_SIZE;
}

/**
 * free block by taddr
 */
static void free_addr(taddr addr) {
	if (get_bits(addr)) {
		set_bits(addr, 0);
	}

	if (!get_bits(addr^1)) {
		free_addr(addr / 2);
	}
}

static void free_down(taddr addr) {
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
static void mark_blocks(taddr block) {
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
static int avail(taddr block) {
	if (block >= maxblocksize) {
		return !get_bits(block) && (block - maxblocksize < sizeofpool);
	}
	return !get_bits(block) && avail(block * 2) && avail(block * 2 + 1);
}

/**
 * check, block is marked or not
 */
static int marked(taddr block) {
	if (block >= maxblocksize) {
		return get_bits(block) && (block - maxblocksize < sizeofpool);
	}
	return get_bits(block) && marked(block * 2) && marked(block * 2 + 1);
}

/**
 * find first proper block
 * addr is addres of first block needed size
 */
static void *find_block_avail(taddr addr) {
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
void *page_alloc(size_t size) {
	size_t size_fr;
	void * ptr = NULL;
//	if (!hasinit) {
//		multipage_init();
//		hasinit = 1;
//	}
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
static void robin_taddr(void *ptr) {
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
void page_free(void *ptr, size_t size) {
	if (ptr == NULL) {
		/* errno = XXX */
		return;
	}
	robin_taddr(ptr);
}

#if 0
/**
 * return list of free and busy blocks in heap
 */
void mpget_blocks_info(struct list_head* list) {
	//TODO:
}
#endif

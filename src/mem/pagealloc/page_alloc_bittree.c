/**
 * @file
 * @brief bit-tree page alloc
 * @details each page represented by one bit;
 * upside every word there is another bit representing if a word is full
 *
 * @date 29.07.11
 * @author Anton Kozlov
 */
#include <types.h>
#include <string.h>
#include <embox/unit.h>

#include <mem/pagealloc/opallocator.h>

typedef uint32_t word_t;

#define NBITS (sizeof(word_t) * 8)
#define NPOWER 5 // XXX = log_2(NBITS)
#define NWORD (CONFIG_MEM_MAX_N_PAGE / NBITS + (CONFIG_MEM_MAX_N_PAGE % NBITS != 0 ? 1 : 0)) // XXX

EMBOX_UNIT_INIT(page_alloc_bittree);

static uint8_t *page_pool = 0;
static int page_num;

static int begin;
static const int root = 0;
static int level;
static int find_free_page(int subroot, int lbegin, int level);
static void mark_page(int pagenum, int mark);

static word_t tree[NWORD * 2 + 1]; // +1 for simplicity

void *page_alloc(void) {
	int res;
	if (tree[root] == -1) {
		return NULL;
	}
	res = find_free_page(root, 0, 0);
	mark_page(res, 1);
	return (void *) (page_pool + res * CONFIG_PAGE_SIZE);
}

void page_free(void *page) {
	int addr = (int) page;
	mark_page((addr - (int) page_pool) / CONFIG_PAGE_SIZE, 0);
}

void find_page_pool(void) {
	extern int _free_mem;
	extern int _mem_begin;
	extern int _mem_length;

	static const int page_mask = (CONFIG_PAGE_SIZE - 1);
	page_pool = (uint8_t *) (((int) &_free_mem) & ((int) ~page_mask));

	if ((int) &_free_mem & page_mask) {
		page_pool += CONFIG_PAGE_SIZE;
	}

	page_num = (((int) &_mem_begin + (int) &_mem_length) - (int) page_pool) / CONFIG_PAGE_SIZE;
}

static int find_first_zero(int val) {
	return ~val & (val + 1);
}

static int get_ls_bit_num(int val) {
	int i = 0x01;
	int j = 0;
	while (0 == (val & i)) {
		i <<= 1;
		j++;
	}
	return j;
}

static int find_free_page(int subroot, int lbegin, int level) {
	int branch, newroot;

	branch = get_ls_bit_num(find_first_zero(tree[lbegin + subroot]));

	if (lbegin == begin) {
		return subroot * NBITS + branch;
	}

	newroot = subroot * NBITS + branch;

	return find_free_page(newroot, lbegin + (1 << ((level) * NPOWER)), level + 1);
}

static void _mark_page(int index, int offset, int begin, int level, int mark) {
	int new_mark;

	if (mark) {
		tree[begin + index] |= 1 << offset;
	} else {
		tree[begin + index] &= ~(1 << offset);
	}

	if (level > 0) {
		new_mark = (tree[begin + index] == -1);
		_mark_page(index / NBITS, index % NBITS, begin - (1 << ((level - 1) * NPOWER)),
				level - 1, new_mark);
	}
}

static void mark_page(int pagenum, int mark) {
	_mark_page(pagenum / NBITS, pagenum % NBITS, begin,	level, mark);
}

static int page_alloc_bittree(void) {
	int tree_page_num = NBITS;
	level = 0;
	begin = 0;

	while (tree_page_num < page_num) {
		begin += tree_page_num / NBITS;
		level ++;
		tree_page_num *= NBITS;
	}

	tree_page_num -= 1;
	while (tree_page_num >= page_num) {
		if (tree_page_num >= NWORD * NBITS) {
			int index = tree_page_num / NBITS;
			_mark_page(index / NBITS, index % NBITS, begin - (1 << ((level - 1) * NPOWER)),
							level - 1, 1);
			tree_page_num -= NBITS;
		} else {
			mark_page(tree_page_num, 1);
			tree_page_num--;
		}
	}

	find_page_pool();

	return 0;
}

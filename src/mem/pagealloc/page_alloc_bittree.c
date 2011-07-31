/**
 * @file
 * @brief bit-tree page alloc
 * @details each page represented by one bit;
 * upside every word there is another bit representing if a word is full
 *
 * @date 29.07.11
 * @author Anton Kozlov
 */

#include <string.h>

#define NPAGE 10
static uint8_t *page_pool;

typedef uint32_t word_t;

#define NBITS (sizeof(word_t) * 8)
#define NWORD (NPAGE / NBITS + (NPAGE % NBITS ? 1 : 0))



EMBOX_UNIT_INIT(page_alloc_bittree);

static int root;
static int find_free_page(int subroot);

void *page_alloc(void) {
	int res;
	if (tree[root] == -1) {
		return NULL;
	}
	res = find_free_page(root);
	return (void *) (page_pool  + res);
}

static word_t tree[NWORD * 2 + 1]; // +1 for simplicity

static void fill_tree(word_t *tree) {
	memcpy(tree, 0, NWORD * 2);
	if (NPAGE % NBITS) {
		for (int i = 31; i >= NPAGE % NBITS; i--) {
			tree[NWORD * 2] |= 1 << i;
		}
	}
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

static int find_free_page(int subroot) {
	int branch, newroot, res;
	branch = get_ls_but_num(find_first_zero(tree[subroot]));
	newroot = subroot * NBITS + branch;
	if (newroot >= NWORD) {
		res = newroot;
		tree[subroot] |= branch;
		if (tree[subroot] != -1) {
			res *= -1;
		}
	} else {
		res = find_free_page(newroot);
	}
	if (res < 0) {
		tree[subroot] |= branch;
		if (tree[subroot] != -1) {
			res *= -1;
		}
	}
	return res;
}


static int page_alloc_bittree(void) {
	fill_tree(&tree);
	root = NWORD; // begin of bit mask for page
	//now are going up for root

	while (root / NBITS) {
		root /= NBITS;
	}
}

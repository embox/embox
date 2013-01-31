/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    31.01.2013
 */

#include <errno.h>
#include <string.h>

#include <fs/xattr.h>
#include <fs/ext2.h>

extern int ext2_close(struct nas *nas);
extern int ext2_open(struct nas *nas);

static inline unsigned int iceil(unsigned int val, unsigned int div) {
	return val + (val % div ? 1 : 0);
}

#define d2h32(v) v
#define h2d32(v) v

#define d2h16(v) v
#define h2d16(v) v

#define d2h8(v) v
#define h2d8(v) v

static int ensure_dinode(struct nas *nas) {
	/* Sorry for doing that (Anton Kozlov).
	 * Needed as mount not filling nas struct,
	 * but postpone it till open
	 */
	ext2_open(nas);
	ext2_close(nas);

	return 0;
}

static int xattr_block(struct node *node, struct ext2_xattr_hdr **blk) {
	struct ext2fs_dinode *dinode;
	struct ext2_xattr_hdr *xattr_blk;
	int res;

	assert(node->nas);

	if (0 != (res = ensure_dinode(node->nas))) {
		return res;
	}

	dinode = node->nas->fi->privdata;

	if (!dinode->i_facl) {
		return -ENOENT;
	}

	if (NULL == (xattr_blk = ext2_buff_alloc(node->nas, BBSIZE))) {
		return -ENOMEM;
	}

	if (0 > (res = ext2_read_sector(node->nas, (char *) xattr_blk, 1,
			d2h32(dinode->i_facl)))) {
		return res;
	}

	if (xattr_blk->h_magic != EXT2_XATTR_HDR_MAGIC) {
		return -EIO;
	}

	*blk = xattr_blk;
	return 0;
}

static inline size_t ent_len(struct ext2_xattr_ent *ent) {
	return sizeof(struct ext2_xattr_ent) +
		iceil(d2h32(ent->e_name_len), EXT2_XATTR_PAD);
}

#define foreach_xattr(_var, _first_entry) \
	for(_var = _first_entry; \
			*(uint32_t *) _var != 0; \
			_var = (struct ext2_xattr_ent *) \
				(((unsigned int) _var) + ent_len(_var)))

int ext2fs_listxattr(struct node *node, char *list, size_t len) {
	struct ext2_xattr_hdr *xattr_blk;
	struct ext2_xattr_ent *xattr_ent;
	int res, last_len = len;
	char check = list == NULL || len == 0;
	char *p = list;

	if (0 != (res = xattr_block(node, &xattr_blk))) {
		if (res == -ENOENT) {
			return 0;
		}
		return res;
	}

	res = 0;
	foreach_xattr(xattr_ent, &xattr_blk->first_entry) {
		if (!check) {
			if (xattr_ent->e_name_len > last_len) {
				return -ERANGE;
			}
			memcpy(p, &xattr_ent->e_name, xattr_ent->e_name_len);
			p += xattr_ent->e_name_len;
			*p++ = '\0';
			last_len -= xattr_ent->e_name_len;
		}
		res += xattr_ent->e_name_len + 1;
	}

	ext2_buff_free(node->nas, (char *) xattr_blk);

	return res;
}

static void del_val(struct ext2_xattr_hdr *xattr_blk, struct ext2_xattr_ent *xattr_ent,
		int *min_value_offs) {
	struct ext2_xattr_ent *i_ent;
	unsigned int dellen = d2h32(xattr_ent->e_value_size);
	unsigned short deloff = d2h16(xattr_ent->e_value_offs);

	memmove(((char*) xattr_blk) + *min_value_offs + dellen,
			((char *) xattr_blk) + *min_value_offs,
			 deloff - *min_value_offs);

	foreach_xattr(i_ent, &xattr_blk->first_entry) {
		unsigned short ent_off = d2h16(i_ent->e_value_offs);
		if (ent_off < deloff) {
			i_ent->e_value_offs = h2d16(ent_off + dellen);
		}
	}
	*min_value_offs += d2h32(xattr_ent->e_value_size);
}

static void del_ent(struct ext2_xattr_hdr *xattr_blk, struct ext2_xattr_ent *xattr_ent,
		struct ext2_xattr_ent *end_ent) {
	void *src = ((char *) xattr_ent) + ent_len(xattr_ent);

	memmove(xattr_ent, src, sizeof(unsigned int) + (unsigned int) end_ent
			- (unsigned int) src);
}

static void str_val(struct ext2_xattr_hdr *xattr_blk, struct ext2_xattr_ent *xattr_ent,
		int *min_value_offs, const char *value, size_t len) {

	*min_value_offs -= len;
	memcpy(((char *) xattr_blk) + *min_value_offs, value, len);
	xattr_ent->e_value_size = h2d32(len);
	xattr_ent->e_value_offs = h2d32(*min_value_offs);
}

int ext2fs_setxattr(struct node *node, const char *name, const char *value,
		size_t len, int flags) {
	struct ext2_xattr_hdr *xattr_blk;
	struct ext2_xattr_ent *xattr_ent, *i_ent;
	struct ext2fs_dinode *dinode = node->nas->fi->privdata;
	int xblk_n, min_value_offs = BBSIZE, name_len = strlen(name);
	int res;

	assert(node->nas);

	if (0 != (res = xattr_block(node, &xattr_blk))) {
		if (res != -ENOENT) {
			return res;
		}

		if (NO_BLOCK == (xblk_n = ext2_alloc_block(node->nas, 0))) {
			return -ENOMEM;
		}

		dinode->i_facl = h2d32(xblk_n);

		ext2_rw_inode(node->nas, dinode, EXT2_W_INODE);

		if (0 != (res = xattr_block(node, &xattr_blk))) {
			return res;
		}
	}

	if (d2h32(xattr_blk->h_refcount) > 1) {
		struct ext2_xattr_hdr *blk_copy;

		if (NO_BLOCK == (xblk_n = ext2_alloc_block(node->nas, 0))) {
			return -ENOMEM;
		}

		blk_copy = ext2_buff_alloc(node->nas, BBSIZE);
		memcpy(blk_copy, xattr_blk, BBSIZE);

		xattr_blk->h_refcount = h2d32(d2h32(xattr_blk->h_refcount) - 1);
		blk_copy->h_refcount = h2d32(1);

		ext2_write_sector(node->nas, (char *) xattr_blk, 1, d2h32(dinode->i_facl));

		dinode->i_facl = h2d32(xblk_n);

		ext2_buff_free(node->nas, (char *) xattr_blk);
		xattr_blk = blk_copy;

		ext2_rw_inode(node->nas, dinode, EXT2_W_INODE);
	}

	xattr_ent = NULL;
	foreach_xattr(i_ent, &xattr_blk->first_entry) {
		if (i_ent->e_value_offs < min_value_offs) {
			min_value_offs = d2h16(i_ent->e_value_offs);
		}

		if (xattr_ent == NULL && name_len == d2h32(i_ent->e_name_len) &&
				0 == strcmp(name, &i_ent->e_name)) {
			xattr_ent = i_ent;
		}
	}

	if (flags & XATTR_CREATE) {
		if (xattr_ent != NULL) {
			return -EEXIST;
		}

		return -ENOSYS;
	} else if (flags & XATTR_REMOVE) {
		if (NULL == xattr_ent) {
			return -ENOENT;
		}

		del_val(xattr_blk, xattr_ent, &min_value_offs);
		del_ent(xattr_blk, xattr_ent, i_ent);

	} else if (flags & XATTR_REPLACE) {
		if (NULL == xattr_ent) {
			return -ENOENT;
		}

		if (((unsigned int) i_ent) + 4 >
				min_value_offs + d2h32(xattr_ent->e_value_size) - len) {
			return -ENOMEM;
		}

		del_val(xattr_blk, xattr_ent, &min_value_offs);
		str_val(xattr_blk, xattr_ent, &min_value_offs, value, len);
	}

	ext2_write_sector(node->nas, (char *) xattr_blk, 1, d2h32(dinode->i_facl));
	ext2_buff_free(node->nas, (char *) xattr_blk);

	return 0;
}

int ext2fs_getxattr(struct node *node, char *name, char *value, size_t len) {
	struct ext2_xattr_hdr *xattr_blk;
	struct ext2_xattr_ent *xattr_ent;
	size_t name_len = strlen(name);
	int res;

	if (0 != (res = xattr_block(node, &xattr_blk))) {
		return res;
	}

	res = -ENOENT;
	foreach_xattr(xattr_ent, &xattr_blk->first_entry) {
		if (name_len == xattr_ent->e_name_len &&
				0 == strcmp(name, &xattr_ent->e_name)) {
			if (xattr_ent->e_value_size < len) {
				strncpy(value, ((char *) xattr_blk) + xattr_ent->e_value_offs,
						xattr_ent->e_value_size);
				res = xattr_ent->e_value_size + 1;
			} else {
				res = -ERANGE;
			}
			break;
		}
	}

	ext2_buff_free(node->nas, (char *) xattr_blk);

	return res;
}


/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    31.01.2013
 */

#include <errno.h>
#include <string.h>

#include <fs/inode.h>

#include <fs/xattr.h>
#include <fs/ext2.h>

extern int ext2_close(struct inode *inode);
extern int ext2_open(struct inode *inode);

static inline unsigned int iceil(unsigned int val, unsigned int div) {
	/* div is 1 << x */
	return (val & ~(div - 1)) + (val & (div - 1) ? div : 0);
}

#define d2h32(v) v
#define h2d32(v) v

#define d2h16(v) v
#define h2d16(v) v

#define d2h8(v) v
#define h2d8(v) v

static inline size_t ent_len(struct ext2_xattr_ent *ent) {
	return sizeof(struct ext2_xattr_ent) +
		iceil(d2h8(ent->e_name_len), EXT2_XATTR_PAD);
}

#define foreach_xattr(_var, _first_entry) \
	for(_var = _first_entry; \
			*(uint32_t *) _var != 0; \
			_var = (struct ext2_xattr_ent *) \
				(((unsigned int) _var) + ent_len(_var)))

#define XATTR_ENT_HASH_NAM_SHIFT 5
#define XATTR_ENT_HASH_NAM_RSHIFT (8 * sizeof(hash) - XATTR_ENT_HASH_NAM_SHIFT)
#define XATTR_ENT_HASH_VAL_SHIFT 16
#define XATTR_ENT_HASH_VAL_RSHIFT (8 * sizeof(hash) - XATTR_ENT_HASH_VAL_SHIFT)

#if __BYTE_ORDER == __BIG_ENDIAN
static void xattrent_bswap(struct ext2_xattr_ent *old, struct ext2_xattr_ent *new) {
	new->e_name_len = old->e_name_len;
	new->e_name_index = old->e_name_index;
	new->e_value_offs = bswap16(old->e_value_offs);
	new->e_value_block = bswap32(old->e_value_block);
	new->e_value_size = bswap32(old->e_value_size);
	new->e_hash = bswap32(old->e_hash);
	memcpy(new->e_name, old->e_name, old->e_name_len);
}

void xattr_bswap(struct ext2_xattr_hdr *old, struct ext2_xattr_hdr *new, size_t size) {
	struct ext2_xattr_ent *i_ent;
	int i = 0;

	new->h_magic = bswap32(old->h_magic);
	new->h_refcount = bswap32(old->h_refcount);
	new->h_blocks = bswap32(old->h_blocks);
	new->h_hash = bswap32(old->h_hash);
	memcpy(&new->reserved, &old->reserved,
			16 * sizeof(uint8_t));
	size -= sizeof(uint32_t) * 4 + sizeof(uint8_t) * 16;

	foreach_xattr(i_ent, old->h_entries) {
		xattrent_bswap(&old->h_entries[i], &new->h_entries[i]);
		i++;
	}
}
#else
void xattr_bswap(struct ext2_xattr_hdr *old, struct ext2_xattr_hdr *new, size_t size) {

}
#endif

static void entry_rehash(struct ext2_xattr_hdr *xattr_blk,
		struct ext2_xattr_ent *xattr_ent) {
	size_t len;

	char *p = xattr_ent->e_name;
	unsigned int *vp = (unsigned int *)
		(((unsigned int) xattr_blk) + d2h16(xattr_ent->e_value_offs));

	unsigned int hash = 0;

	len = xattr_ent->e_name_len;
	while (len--) {
		hash = (hash << XATTR_ENT_HASH_NAM_SHIFT) ^
			(hash >> XATTR_ENT_HASH_NAM_RSHIFT) ^
			*p++;
	}

	if (xattr_ent->e_value_block == 0 && xattr_ent->e_value_size != 0) {
		len = iceil(d2h32(xattr_ent->e_value_size), EXT2_XATTR_PAD) >> 2;
		while (len--) {
			hash = (hash << XATTR_ENT_HASH_VAL_SHIFT) ^
				(hash >> XATTR_ENT_HASH_VAL_RSHIFT) ^
				d2h32(*vp++);
		}
	}
	xattr_ent->e_hash = h2d32(hash);
}

static int ensure_dinode(struct inode *inode) {
	/* Sorry for doing that (Anton Kozlov).
	 * Needed as mount not filling nas struct,
	 * but postpone it till open
	 */
	ext2_open(inode);
	ext2_close(inode);

	return 0;
}

static int xattr_block(struct inode *node, struct ext2_xattr_hdr **blk,
		char check_magic) {
	struct ext2fs_dinode *dinode;
	struct ext2_xattr_hdr *xattr_blk;
	struct super_block *sb;
	int res;

	if (0 != (res = ensure_dinode(node))) {
		return res;
	}

	dinode = inode_priv(node);

	if (!dinode->i_facl) {
		return -ENOENT;
	}

	sb = node->i_sb;
	if (NULL == (xattr_blk = ext2_buff_alloc(sb->sb_data, BBSIZE))) {
		return -ENOMEM;
	}

	if (0 > (res = ext2_read_sector(sb, (char *) xattr_blk, 1,
			d2h32(dinode->i_facl)))) {
		ext2_buff_free(sb->sb_data, (char *) xattr_blk);
		return res;
	}

	xattr_bswap(xattr_blk, xattr_blk, SECTOR_SIZE);

	if (check_magic && d2h32(xattr_blk->h_magic) != EXT2_XATTR_HDR_MAGIC) {
		ext2_buff_free(sb->sb_data, (char *) xattr_blk);
		return -EIO;
	}

	*blk = xattr_blk;
	return 0;
}

#define XATTR_BLK_HASH_SHIFT 16
#define XATTR_BLK_HASH_RSHIFT (8 * sizeof(unsigned int) - XATTR_BLK_HASH_SHIFT)

static void block_rehash(struct ext2_xattr_hdr *xattr_blk) {
	struct ext2_xattr_ent *i_ent;

	unsigned int hash = 0;

	foreach_xattr(i_ent, xattr_blk->h_entries) {
		hash = (hash << XATTR_BLK_HASH_SHIFT) ^
			(hash >> XATTR_BLK_HASH_RSHIFT) ^
			d2h32(i_ent->e_hash);
	}

	xattr_blk->h_hash = h2d32(hash);
}

int ext2fs_listxattr(struct inode *node, char *list, size_t len) {
	struct ext2_xattr_hdr *xattr_blk;
	struct ext2_xattr_ent *xattr_ent;
	int res, last_len = len;
	char check = list == NULL || len == 0;
	char *p = list;

	if (0 != (res = xattr_block(node, &xattr_blk, 1))) {
		if (res == -ENOENT) {
			return 0;
		}
		return res;
	}

	res = 0;
	foreach_xattr(xattr_ent, xattr_blk->h_entries) {
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

	ext2_buff_free(node->i_sb->sb_data, (char *) xattr_blk);

	return res;
}

static void del_val(struct ext2_xattr_hdr *xattr_blk,
		struct ext2_xattr_ent *xattr_ent, int *min_value_offs) {
	struct ext2_xattr_ent *i_ent;
	unsigned int dellen = iceil(d2h32(xattr_ent->e_value_size),
			EXT2_XATTR_PAD);
	unsigned short deloff = d2h16(xattr_ent->e_value_offs);

	memmove(((char*) xattr_blk) + *min_value_offs + dellen,
			((char *) xattr_blk) + *min_value_offs,
			 deloff - *min_value_offs);

	foreach_xattr(i_ent, xattr_blk->h_entries) {
		unsigned short ent_off = d2h16(i_ent->e_value_offs);
		if (ent_off < deloff) {
			i_ent->e_value_offs = h2d16(ent_off + dellen);
			entry_rehash(xattr_blk, i_ent);
		}
	}
	*min_value_offs += dellen;
}

static void del_ent(struct ext2_xattr_hdr *xattr_blk,
		struct ext2_xattr_ent *xattr_ent,
	       	struct ext2_xattr_ent *end_ent) {
	void *src = ((char *) xattr_ent) + ent_len(xattr_ent);

	memmove(xattr_ent, src, EXT2_XATTR_PAD + (unsigned int) end_ent
			- (unsigned int) src);
}

static struct ext2_xattr_ent * str_ent(struct ext2_xattr_hdr *xattr_blk,
		struct ext2_xattr_ent *end_ent, const char *name) {
	struct ext2_xattr_ent *i_ent;
	size_t name_len = strlen(name);

	foreach_xattr(i_ent, xattr_blk->h_entries) {
		if (strncmp(name, i_ent->e_name, i_ent->e_name_len) < 0) {
			break;
		}
	}

	memmove(((void *) i_ent) + sizeof(struct ext2_xattr_ent)
				+ iceil(strlen(name), EXT2_XATTR_PAD),
			i_ent,
			((unsigned int) end_ent) - ((unsigned int) i_ent)
				+ EXT2_XATTR_PAD);

	i_ent->e_name_len = (uint8_t) name_len;
	i_ent->e_name_index = 1;
	i_ent->e_value_block = 0;

	memcpy(i_ent->e_name, name, name_len);

	return i_ent;
}


static void str_val(struct ext2_xattr_hdr *xattr_blk,
		struct ext2_xattr_ent *xattr_ent, int *min_value_offs,
		const char *value, size_t len) {

	size_t alen = iceil(len, EXT2_XATTR_PAD);

	*min_value_offs -= alen;
	memcpy(((char *) xattr_blk) + *min_value_offs, value, len);
	xattr_ent->e_value_size = h2d32(len);
	xattr_ent->e_value_offs = h2d32(*min_value_offs);
}

int ext2fs_setxattr(struct inode *node, const char *name, const char *value,
		size_t len, int flags) {
	struct ext2_xattr_hdr *xattr_blk = NULL;
	struct ext2_xattr_ent *xattr_ent, *i_ent;
	struct ext2fs_dinode *dinode = inode_priv(node);
	int xblk_n, min_value_offs = BBSIZE, name_len = strlen(name);
	int res = 0;
	struct super_block *sb;

	sb = node->i_sb;

	if (0 != (res = xattr_block(node, &xattr_blk, 1))) {
		if (res != -ENOENT) {
			return res;
		}

		if (NO_BLOCK == (xblk_n = ext2_alloc_block(node, 0))) {
			res = -ENOMEM;
			goto cleanup_out;
		}

		if ((res = ext2_write_gdblock(sb))) {
			goto cleanup_out;
		}

		if ((res = ext2_write_sblock(sb))) {
			goto cleanup_out;
		}

		{
			struct ext2_fs_info *fsi = sb->sb_data;
			dinode->i_facl = h2d32(xblk_n);
			dinode->i_blocks = h2d32(d2h32(dinode->i_blocks) +
					fsi->s_sectors_in_block);
		}

		ext2_rw_inode(node, dinode, EXT2_W_INODE);

		if (0 != (res = xattr_block(node, &xattr_blk, 0))) {
			goto cleanup_out;
		}

		xattr_blk->h_magic = h2d32(EXT2_XATTR_HDR_MAGIC);
		xattr_blk->h_refcount = h2d32(1);
		xattr_blk->h_blocks = h2d32(1);
		memset(xattr_blk->h_entries, 0, EXT2_XATTR_PAD);
	}

	if (d2h32(xattr_blk->h_refcount) > 1) {
		struct ext2_xattr_hdr *blk_copy;

		if (NO_BLOCK == (xblk_n = ext2_alloc_block(node, 0))) {
			res = -ENOMEM;
			goto cleanup_out;
		}

		if (NULL == (blk_copy = ext2_buff_alloc(sb->sb_data, BBSIZE))) {
			res = -ENOMEM;
			goto cleanup_out;
		}

		memcpy(blk_copy, xattr_blk, BBSIZE);

		xattr_blk->h_refcount = h2d32(d2h32(xattr_blk->h_refcount) - 1);
		block_rehash(xattr_blk);

		blk_copy->h_refcount = h2d32(1);

		xattr_bswap(xattr_blk, xattr_blk, SECTOR_SIZE);

		if ((res = ext2_write_sector(sb, (char *) xattr_blk,
						1, d2h32(dinode->i_facl)))) {
			res = -EIO;
			goto cleanup_out;
		}

		dinode->i_facl = h2d32(xblk_n);
		/* TODO: should we alter i_blocks* */

		ext2_buff_free(sb->sb_data, (char *) xattr_blk);
		xattr_blk = blk_copy;

		ext2_rw_inode(node, dinode, EXT2_W_INODE);
	}

	xattr_ent = NULL;
	foreach_xattr(i_ent, xattr_blk->h_entries) {
		if (i_ent->e_value_offs < min_value_offs) {
			min_value_offs = d2h16(i_ent->e_value_offs);
		}

		if (xattr_ent == NULL && name_len == d2h8(i_ent->e_name_len) &&
				0 == strncmp(name, i_ent->e_name, name_len)) {
			xattr_ent = i_ent;
		}
	}

	if (!flags) {
		flags |= xattr_ent ? XATTR_REPLACE : XATTR_CREATE;
	}

	if (flags & XATTR_CREATE) {
		if (xattr_ent != NULL) {
			res = -EEXIST;
			goto cleanup_out;
		}

		if (((unsigned int) i_ent) - ((unsigned int) xattr_blk)
				+ sizeof(struct ext2_xattr_ent) +
				iceil(strlen(name), EXT2_XATTR_PAD) + 4
				> min_value_offs - iceil(len, EXT2_XATTR_PAD)) {
			res = -ENOMEM;
			goto cleanup_out;
		}

		if (strlen(name) > ((uint8_t) -1)) {
			res = -ENOMEM;
			goto cleanup_out;
		}

		i_ent = str_ent(xattr_blk, i_ent, name);
		str_val(xattr_blk, i_ent, &min_value_offs, value, len);

		entry_rehash(xattr_blk, i_ent);

	} else if (flags & XATTR_REMOVE) {
		if (NULL == xattr_ent) {
			res = -ENOENT;
			goto cleanup_out;
		}

		del_val(xattr_blk, xattr_ent, &min_value_offs);
		del_ent(xattr_blk, xattr_ent, i_ent);

	} else if (flags & XATTR_REPLACE) {
		if (NULL == xattr_ent) {
			res = -ENOENT;
			goto cleanup_out;
		}

		if (((unsigned int) i_ent) - ((unsigned int) xattr_blk) + 4
				> min_value_offs
					+ iceil(d2h32(xattr_ent->e_value_size),
					    EXT2_XATTR_PAD) - len) {
			res = -ENOMEM;
			goto cleanup_out;
		}

		del_val(xattr_blk, xattr_ent, &min_value_offs);
		str_val(xattr_blk, xattr_ent, &min_value_offs, value, len);

		entry_rehash(xattr_blk, xattr_ent);
	}

	block_rehash(xattr_blk);

	ext2_write_sector(sb, (char *) xattr_blk, 1,
			d2h32(dinode->i_facl));

cleanup_out:
	if (xattr_blk) {
		ext2_buff_free(sb->sb_data, (char *) xattr_blk);
	}

	return res;
}

int ext2fs_getxattr(struct inode *node, const char *name, char *value,
		size_t len) {
	struct ext2_xattr_hdr *xattr_blk;
	struct ext2_xattr_ent *xattr_ent;
	size_t name_len = strlen(name);
	int res;

	if (0 != (res = xattr_block(node, &xattr_blk, 1))) {
		return res;
	}

	res = -ENOENT;
	foreach_xattr(xattr_ent, xattr_blk->h_entries) {
		if (name_len == xattr_ent->e_name_len &&
				0 == strncmp(name, xattr_ent->e_name,
						strlen(name))) {
			if (xattr_ent->e_value_size < len) {
				const char *vsrc = ((char *) xattr_blk)
					+ xattr_ent->e_value_offs;

				strncpy(value, vsrc, xattr_ent->e_value_size);
				*(value + xattr_ent->e_value_size) = '\0';
				res = xattr_ent->e_value_size + 1;
			} else if (len == 0 || value == NULL) {
				res = xattr_ent->e_value_size + 1;
			} else {
				res = -ERANGE;
			}
			break;
		}
	}

	ext2_buff_free(node->i_sb->sb_data, (char *) xattr_blk);

	return res;
}


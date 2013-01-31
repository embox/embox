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
	return val / div  + (val % div ? 1 : 0);
}

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
		return 0;
	}

	if (NULL == (xattr_blk = ext2_buff_alloc(node->nas, BBSIZE))) {
		return -ENOMEM;
	}

	if (0 > (res = ext2_read_sector(node->nas, (char *) xattr_blk, 1, dinode->i_facl))) {
		return res;
	}

	if (xattr_blk->h_magic != EXT2_XATTR_HDR_MAGIC) {
		return -EIO;
	}

	*blk = xattr_blk;
	return 0;
}

#define foreach_xattr(_var, _first_entry) \
	for(struct ext2_xattr_ent *xattr_ent = &_first_entry; \
			*(uint32_t *) _var != 0; \
			_var = (struct ext2_xattr_ent *) \
				((unsigned int) _var) + \
				   sizeof(struct ext2_xattr_ent) +\
				   iceil(_var->e_name_len, 4))

int ext2fs_listxattr(struct node *node, char *list, size_t len) {
	struct ext2_xattr_hdr *xattr_blk;
	int res, last_len = len;
	char check = list == NULL || len == 0;
	char *p = list;

	if (0 != (res = xattr_block(node, &xattr_blk))) {
		return res;
	}

	res = 0;
	foreach_xattr(xattr_ent, xattr_blk->first_entry) {
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

int ext2fs_setxattr(struct node *node, const char *name, const char *value,
		size_t len, int flags) {
	return -ENOSYS;
}

int ext2fs_getxattr(struct node *node, char *name, char *value, size_t len) {
	struct ext2_xattr_hdr *xattr_blk;
	size_t name_len = strlen(name);
	int res;

	if (0 != (res = xattr_block(node, &xattr_blk))) {
		return res;
	}

	res = -ENOENT;
	foreach_xattr(xattr_ent, xattr_blk->first_entry) {
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


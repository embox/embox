/**
* @file dvfs_dentry.c
* @brief Handle dentry routines
* @author Denis Deryugin <deryugin.denis@gmail.com>
* @version 0.1
* @date 2015-06-01
*/

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <embox/block_dev.h>
#include <framework/mod/options.h>
#include <fs/dvfs.h>
#include <fs/hlpr_path.h>
#include <kernel/task/resource/vfs.h>

#define DCACHE_ENABLED OPTION_GET(BOOLEAN, use_dcache)
#define DENTRY_POOL_SIZE OPTION_GET(NUMBER, dentry_pool_size)

#if DCACHE_ENABLED

#include <mem/misc/pool.h>
#include <util/hashtable.h>

static const unsigned long long prime = 19, module = 1023;
static unsigned long long pows[DENTRY_NAME_LEN];

static void hash_init(void) {
	/* TODO precount in runtime or even in compiletime */
	int i;
	pows[0] = 1;
	for (i = 1; i < DENTRY_NAME_LEN; i++)
		pows[i] = (pows[i - 1] * prime) % module;
}

static unsigned long long poly_hash(const char *str) {
	unsigned long long res = 0;
	const char *pos = str;

	hash_init();

	while (*pos) {
		res += ((int) *str) * pows[pos - str];
		res %= module;
		pos++;
	}
	return res;
}

static size_t get_dentry_hash(void *key) {
	return (size_t) key;
}

static int cmp_dentries(void *key1, void *key2) {
	return !(key1 == key2);
}

HASHTABLE_DEF(dentry_ht,
		DENTRY_POOL_SIZE * sizeof(struct dentry *),
		get_dentry_hash,
		cmp_dentries);

POOL_DEF(dentry_ht_pool, struct hashtable_item, DENTRY_POOL_SIZE);
#endif

extern int dentry_fill(struct super_block *, struct inode *,
                       struct dentry *, struct dentry *);
extern struct dentry *local_lookup(struct dentry *parent, char *name);

/**
 * @brief Get the length of next element int the path
 * @param path Pointer to the path
 *
 * @return The length of next element in the path
 * @revtal -1 Error
 */
int dvfs_path_next_len(const char *path) {
	int len = strlen(path);
	int off = 0;

	while ((path[off] != '/') && (off < len))
		off++;

	return off;
}
/**
 * @brief Resolve one more element in the path
 * @param path   Pointer to relative path
 * @param dentry The previous dentry
 * @param lookup Structure which is to contain result of path walk
 *
 * @return Negative error code
 * @retval       0 Ok
 * @retval -ENOENT Node not found
 */
int dvfs_path_walk(const char *path, struct dentry *parent, struct lookup *lookup) {
	char buff[DENTRY_NAME_LEN];
	struct inode *in;
	int len;
	struct dentry *d;
	assert(parent);
	assert(path);

	while (*path == '/')
		path++;

	len = dvfs_path_next_len(path);
	memcpy(buff, path, len);
	buff[len] = '\0';

	if (buff[0] == '\0') {
		*lookup = (struct lookup) {
			.item   = parent,
			.parent = parent->parent,
		};
		return 0;
	}

	if ((d = local_lookup(parent, buff)))
		return dvfs_path_walk(path + strlen(buff), d, lookup);

	if (strlen(buff) > 1 && path_is_double_dot(buff))
		return dvfs_path_walk(path + 2, parent->parent, lookup);

	if (strlen(buff) > 1 && path_is_single_dot(buff))
		return dvfs_path_walk(path + 2, parent, lookup);

	/* TODO use cache instead */
	assert(parent->d_sb);
	assert(parent->d_sb->sb_iops);
	assert(parent->d_sb->sb_iops->lookup);

	if (!(in = parent->d_sb->sb_iops->lookup(buff, parent))) {
		*lookup = (struct lookup) {
			.item   = NULL,
			.parent = parent,
		};
		return -ENOENT;
	} else {
		struct dentry *d;
		d = dvfs_alloc_dentry();
		in->i_dentry = parent;
		dentry_fill(parent->d_sb, in, d, parent);
		strcpy(d->name, buff);
		d->flags = in->flags;
	}

	return dvfs_path_walk(path + strlen(buff), in->i_dentry, lookup);
}

/* DVFS interface */

/**
 * @brief Add dentry to cache
 *
 * @param dentry
 * @param hash Could be calculated from dentry, remove may be?
 * @return Negative error code
 */
int dvfs_cache_add(struct dentry *dentry, unsigned long long hash) {
	struct hashtable_item *ht_item = pool_alloc(&dentry_ht_pool);
	ht_item = hashtable_item_init(ht_item, (void *) *((size_t *) &hash), dentry);
	hashtable_put(&dentry_ht, ht_item);
	return 0;
}

/**
 * @brief Remove dentry from cache
 * @note Should be used only on unmount and dentry destroy
 *
 * @param dentry
 *
 * @return Negative error code
 */
int dvfs_cache_del(struct dentry *dentry, unsigned long long hash) {
	hashtable_del(&dentry_ht, (void *) *((size_t *) &hash));
	return 0;
}

/**
 * @brief Try to find dentry at specified path
 * @param path   Absolute or relative path
 * @param lookup Structure where result will be stored
 *
 * @return Negative error code
 * @retval       0 Ok
 * @retval -ENOENT No node found or incorrect root/pwd dentry
 */
int dvfs_lookup(const char *path, struct lookup *lookup) {
	struct dentry *dentry;
	int errcode;

	if (path[0] == '/') {
		dentry = task_fs()->root;
		path++;
	} else
		dentry = task_fs()->pwd;

	if (dentry->d_sb == NULL)
		return -ENOENT;

#if DCACHE_ENABLED
	/* Cache lookup */
	unsigned long long hash = poly_hash(path);
	struct dentry *res;
	/* TODO local path hash offset */
	res = hashtable_get(&dentry_ht, (void *) *((size_t *) &hash));
	if (res) {
		/* TODO walk to root to check */
		*lookup = (struct lookup) {
			.item = res,
			.parent = res->parent,
		};
		return 0;
	}
#endif
	errcode = dvfs_path_walk(path, dentry, lookup);
#if DCACHE_ENABLED
	dvfs_cache_add(lookup->item, hash);
#endif

	return errcode;
}

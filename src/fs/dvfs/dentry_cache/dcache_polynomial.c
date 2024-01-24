/**
 * @file
 * @brief Cache strategy using polynomial hashes to retrive dentries
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-06-09
 */

#include <string.h>
#include <limits.h>

#include <fs/dvfs.h>
#include <kernel/printk.h>
#include <mem/misc/pool.h>
#include <lib/libds/hashtable.h>

/* TODO remove this */
#define DENTRY_POOL_SIZE 64

static const unsigned long long prime = 19, module = 1023;
static unsigned long long pows[PATH_MAX];

static void hash_init(void) {
	/* TODO precount in runtime or even in compiletime */
	int i;
	pows[0] = 1;
	for (i = 1; i < PATH_MAX; i++)
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

extern int dentry_full_path(struct dentry *dentry, char *buf);

/**
 * @brief Add dentry to cache
 *
 * @param dentry
 * @return Negative error code
 */
int dvfs_cache_add(struct dentry *dentry) {
	char pathname[PATH_MAX];
	unsigned long long hash;
	struct hashtable_item *ht_item = pool_alloc(&dentry_ht_pool);
	assert(ht_item);
	dentry_full_path(dentry, pathname);
	hash = poly_hash(pathname);
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
int dvfs_cache_del(struct dentry *dentry) {
	char pathname[PATH_MAX];
	unsigned long long hash;
	struct hashtable_item *ht_item;
	if (dentry->name[0] == '\0')
		return -1;
	dentry_full_path(dentry, pathname);
	hash = poly_hash(pathname);
	if (!(ht_item = hashtable_del(&dentry_ht, (void *) *((size_t *) &hash)))) {
		printk("Remove empty dentry\n");
	}
	pool_free(&dentry_ht_pool, ht_item);
	return 0;
}

/**
 * @brief Try to get dentry with given path from cache
 *
 * @param path
 *
 * @return
 */
struct dentry *dvfs_cache_get(char *path, struct lookup *lookup) {
	unsigned long long hash = poly_hash(path);
	struct dentry *res;
	/* TODO local path hash offset */
	res = hashtable_get(&dentry_ht, (void *) *((size_t *) &hash));
	return res;
}

struct dentry *dvfs_cache_lookup(const char *path,
	struct dentry *base) {
	char full_path[PATH_MAX];
	struct dentry *ret;

	dentry_full_path(base, full_path);
	strncat(full_path, path, PATH_MAX - strlen(path));

	/* TODO check path from root */

	ret = dvfs_cache_get(full_path, NULL);
	dentry_ref_inc(ret);

	return ret;
}

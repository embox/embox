/**
 * @file
 *
 * @date 29.04.14
 * @author Vita Loginova
 */
#include <util/hashtable.h>
#include <util/dlist.h>
#include <errno.h>
#include <embox/unit.h>
#include <string.h>

#include <mem/misc/pool.h>

#include <dirent.h>

#include <fs/dcache.h>

#define DCACHE_TABLE_SIZE    10
#define DCACHE_MAX_NAME_SIZE 20

EMBOX_UNIT_INIT(dcache_init);

//todo may be localname + mount_table link
struct dkey {
	char fullpath[DCACHE_MAX_NAME_SIZE + 1];
};

struct dvalue {
	struct dkey key;
	struct path path;
	struct dlist_head link;
};

POOL_DEF(dcache_path_pool, struct dvalue, DCACHE_TABLE_SIZE);

static struct hashtable *dcache_table = NULL;
static struct dlist_head values;

static void dvalue_delete(struct dvalue *dvalue) {
	hashtable_del(dcache_table, &dvalue->key);
	dlist_del(&dvalue->link);
	pool_free(&dcache_path_pool, dvalue);
}

static void dvalue_init(struct dvalue *dvalue, struct path *path, const char *fullpath) {
	strcpy(dvalue->key.fullpath, fullpath);
	dvalue->path = *path;
	dlist_head_init(&dvalue->link);
}

static void dvalue_add(struct dvalue *dvalue) {
	dlist_add_next(&dvalue->link, &values);
	hashtable_put(dcache_table, &dvalue->key, dvalue);
}

int dcache_add(const char *fullpath, struct path *path) {
	struct dvalue *dvalue;

	assert(path);

	if (*fullpath == '\0') {
		return -1;
	}

	if (strlen(fullpath) > DCACHE_MAX_NAME_SIZE) {
		return -1;
	}

	if (NULL == (dvalue = pool_alloc(&dcache_path_pool))) {
		struct dvalue *last;

		last = dlist_entry(values.prev, struct dvalue, link);
		dvalue_delete(last);
		dvalue = pool_alloc(&dcache_path_pool);

		assert(NULL != dvalue);
	}

	dvalue_init(dvalue, path, fullpath);
	dvalue_add(dvalue);

	return 0;
}

struct path *dcache_get(const char *fullpath) {
	struct dkey dkey;
	struct dvalue *dvalue;

	if (*fullpath == '\0') {
		return NULL;
	}

	strcpy(dkey.fullpath, fullpath);
	dvalue = hashtable_get(dcache_table, &dkey);

	return dvalue ? &dvalue->path : NULL;
}

static size_t dcache_hash(struct dkey *dkey) {
	size_t hash;
	char *path = dkey->fullpath;

	hash = 0;
	while (*path != '\0') {
		hash ^= *path++;
	}

	return hash;
}

static int dcache_cmp(struct dkey *key1, struct dkey *key2) {
	return strcmp(key1->fullpath, key2->fullpath);
}

static int dcache_init(void) {
	dcache_table = hashtable_create(DCACHE_TABLE_SIZE,
			(get_hash_ft) &dcache_hash, (ht_cmp_ft) &dcache_cmp);

	if (dcache_table == NULL) {
		return -ENOMEM;
	}

	dlist_init(&values);

	return 0;
}


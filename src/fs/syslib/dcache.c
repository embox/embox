/**
 * @file
 *
 * @date 29.04.14
 * @author Vita Loginova
 */

#include <errno.h>
#include <dirent.h>
#include <string.h>

#include <fs/dcache.h>
#include <fs/dvfs.h>
#include <mem/misc/pool.h>
#include <util/hashtable.h>
#include <util/dlist.h>

#include <embox/unit.h>

#define DCACHE_TABLE_SIZE OPTION_GET(NUMBER, dcache_table_size)
#define DCACHE_SIZE       OPTION_GET(NUMBER, dcache_size)
#define DCACHE_MAX_NAME_SIZE 20

EMBOX_UNIT_INIT(dcache_init);

struct dkey {
	char fullpath[DCACHE_MAX_NAME_SIZE + 1];
};

struct dvalue {
	struct dkey key;
	struct dentry dentry;
	struct dlist_head link;
};

POOL_DEF(dcache_dentry_pool, struct dvalue, DCACHE_TABLE_SIZE);
POOL_DEF(ht_item_pool, struct hashtable_item, DCACHE_SIZE);

static size_t dcache_hash(void *key);
static int dcache_cmp(void *key1, void *key2);

HASHTABLE_DEF(dcache_ht, DCACHE_TABLE_SIZE,	&dcache_hash, &dcache_cmp);

static struct hashtable *dcache_table = &dcache_ht;
static struct dlist_head values;

static size_t dcache_hash(void *key) {
	struct dkey *dkey = key;
	size_t hash;
	char *path = dkey->fullpath;

	hash = 0;
	while (*path != '\0') {
		hash ^= *path++;
	}

	return hash;
}

static int dcache_cmp(void *key1, void *key2) {
	struct dkey *dkey1 = key1, *dkey2 = key2;
	return strcmp(dkey1->fullpath, dkey2->fullpath);
}

static int dcache_lazy_init(void) {

	dlist_init(&values);

	return 0;
}

static void dvalue_delete(struct dvalue *dvalue) {
	struct hashtable_item *ht_item;

	ht_item = hashtable_del(dcache_table, &dvalue->key);
	pool_free(&ht_item_pool, ht_item);
	dlist_del(&dvalue->link);
	pool_free(&dcache_dentry_pool, dvalue);
}

static void compound_dentry(char *source, const char *prefix, const char *rest) {
	strcpy(source, prefix);

	if (rest[0] != '/' && (0 != strcmp("/", prefix))) {
		strcat(source, "/");
	}

	strcat(source, rest);
}

static void dvalue_init(struct dvalue *dvalue, struct dentry *dentry,
		const char *prefix, const char *rest) {
	compound_dentry(dvalue->key.fullpath, prefix, rest);
	dvalue->dentry = *dentry;
	dlist_head_init(&dvalue->link);
}

static void dvalue_add(struct dvalue *dvalue) {
	struct hashtable_item *ht_item;

	dlist_add_next(&dvalue->link, &values);

	ht_item = pool_alloc(&ht_item_pool);
	ht_item = hashtable_item_init(ht_item, &dvalue->key, dvalue);
	hashtable_put(dcache_table, ht_item);

}

int dcache_delete(const char *prefix, const char *rest) {
	struct dkey dkey;
	struct dvalue *dvalue;

	if (strlen(rest) == 0) {
		return -EINVAL;
	}

	if (strlen(prefix) + strlen(rest) + 1 > DCACHE_MAX_NAME_SIZE) {
		return -EINVAL;
	}

	compound_dentry(dkey.fullpath, prefix, rest);
	if (NULL != (dvalue = hashtable_get(dcache_table, &dkey))) {
		dvalue_delete(dvalue);
	}

	return 0;
}

int dcache_add(const char *prefix, const char *rest, struct dentry *dentry) {
	struct dvalue *dvalue;
	int res;


	if (!dentry) {
		return -EINVAL;
	}

	if (strlen(rest) == 0) {
		return -EINVAL;
	}

	if (strlen(prefix) + strlen(rest) + 1 > DCACHE_MAX_NAME_SIZE) {
		return -EINVAL;
	}

	if (NULL == dcache_table) {
		 res = dcache_lazy_init();
		 if (res != 0) {
			 return res;
		 }
	}

	if (NULL == (dvalue = pool_alloc(&dcache_dentry_pool))) {
		struct dvalue *last;

		last = dlist_entry(values.prev, struct dvalue, link);
		dvalue_delete(last);
		dvalue = pool_alloc(&dcache_dentry_pool);

		assert(NULL != dvalue);
	}

	dvalue_init(dvalue, dentry, prefix, rest);
	dvalue_add(dvalue);

	return 0;
}

struct dentry *dcache_get(const char *prefix, const char *rest) {
	struct dkey dkey;
	struct dvalue *dvalue;
	int prefix_len, rest_len;
	const char *pref_tmp;
	int res;

	prefix_len = 0;

	if (rest == NULL || 0 == (rest_len = strlen(rest))) {
		return NULL;
	}

	if (prefix == NULL) {
		prefix_len = 0;
		pref_tmp = "";
	} else {
		prefix_len = strlen(prefix);
		pref_tmp = prefix;
	}
	if ((prefix_len + rest_len + 1) > DCACHE_MAX_NAME_SIZE) {
		return NULL;
	}
	if (NULL == dcache_table) {
		 res = dcache_lazy_init();
		 if (res != 0) {
			 return NULL;
		 }
	}

	compound_dentry(dkey.fullpath, pref_tmp, rest);
	dvalue = hashtable_get(dcache_table, &dkey);

	return dvalue ? &dvalue->dentry : NULL;
}

static int dcache_init(void) {
	if (NULL == dcache_table) {
		return dcache_lazy_init();
	}

	return 0;
}


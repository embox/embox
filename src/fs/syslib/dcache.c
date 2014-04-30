/**
 * @file
 *
 * @date 29.04.14
 * @author Vita Loginova
 */
#include <util/hashtable.h>
#include <errno.h>
#include <embox/unit.h>
#include <string.h>

#include <mem/misc/pool.h>

#include <dirent.h>

#include <fs/dcache.h>

#define DCACHE_TABLE_SIZE    10
#define DCACHE_MAX_NAME_SIZE 20

EMBOX_UNIT_INIT(dcache_init);

POOL_DEF(dcache_path_pool, struct path, DCACHE_TABLE_SIZE);

struct hashtable *dcache_table = NULL;

//todo may be localname + mount_table link
struct dcache {
	char fullpath[DCACHE_MAX_NAME_SIZE];
};

int dcache_add(const char *fullpath, struct path *path) {
	struct dcache dcache;
	struct path *dpath;

	*dcache.fullpath = *fullpath;

	if(NULL == (dpath = pool_alloc(&dcache_path_pool))) {
		return -1;
	}

	*dpath = *path;

	hashtable_put(dcache_table, &dcache, dpath);

	return 0;
}

struct path *dcache_get(const char *fullpath) {
	struct dcache dcache;

	if (fullpath == '\0') {
		return 0;
	}

	*dcache.fullpath = *fullpath;
	return hashtable_get(dcache_table, &dcache);
}


static size_t dcache_hash(struct dcache *dcache) {
	size_t hash;
	char *path = &dcache->fullpath[0];

	hash = 0;
	while (*path != '\0') {
		hash ^= *path++;
	}

	return hash;
}

static int dcache_cmp(struct dcache *key1, struct dcache *key2) {
	return strcmp(&key1->fullpath[0], &key1->fullpath[0]);
}

static int dcache_init(void) {
	dcache_table = hashtable_create(DCACHE_TABLE_SIZE,
				(get_hash_ft)&dcache_hash, (ht_cmp_ft)&dcache_cmp);
		if (dcache_table == NULL) {
			return -ENOMEM;
		}

		return 0;
}


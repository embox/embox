/**
 * @file
 *
 * @brief
 *
 * @date 25.11.2011
 * @author Anton Bondarev
 */

#include <embox/test.h>
#include <mem/misc/pool.h>

#define OBJECTS_QUANTITY 0x10

struct test_obj {
	int a;
	char b;
};

POOL_DEF(pool, struct test_obj, OBJECTS_QUANTITY);

EMBOX_TEST_SUITE("fixed size pool test");


TEST_CASE("single object allocation") {
	struct test_obj *obj;
	obj = pool_alloc(&pool);
	test_assert_not_null(obj);
	pool_free(&pool, obj);
}

TEST_CASE("test object freeing") {
	struct test_obj *objs[OBJECTS_QUANTITY + 1];
	int i;
	for(i = 0; i < OBJECTS_QUANTITY; i ++) {
		if(NULL == (objs[i] = pool_alloc(&pool))) {
			break;
		}
	}
	test_assert(i == OBJECTS_QUANTITY);
	pool_free(&pool, objs[0]);
	objs[i] = pool_alloc(&pool);
	test_assert_not_null(objs[OBJECTS_QUANTITY]);
	for (; i > 0; --i) {
		pool_free(&pool, objs[i]);
	}
}
/*
 *
 */

struct object {
	char some_stuff[5];
};

#define MY_POOL_SZ 13

POOL_DEF(my_pool, struct object, MY_POOL_SZ);
static struct object *objects[MY_POOL_SZ];

static void do_alloc(struct pool *, struct object *objects[], int nr);
static void do_free(struct pool *, struct object *objects[], int nr);

TEST_CASE("pool_alloc should return NULL when the pool becomes full") {
	do_alloc(&my_pool, objects, MY_POOL_SZ);
	test_assert_null(pool_alloc(&my_pool));
	do_free(&my_pool, objects, MY_POOL_SZ);
}

TEST_CASE("After freeing all objects using pool_free one should be able to "
		"allocate the same number of objects again") {
	for (int i = 0; i < 2; ++i) {
		do_alloc(&my_pool, objects, MY_POOL_SZ);
		test_assert_null(pool_alloc(&my_pool));
		do_free(&my_pool, objects, MY_POOL_SZ);
	}
}

TEST_CASE("All object should belong to pool") {
	struct object *obj = NULL;
	do_alloc(&my_pool, objects, MY_POOL_SZ);
	for (int i = 0; i < MY_POOL_SZ; ++i) {
		test_assert_true(pool_belong(&my_pool, objects[i]));
	}

	obj = my_pool.memory + my_pool.pool_size;

	for (int i = 0; i < MY_POOL_SZ; ++i) {
		test_assert_false(pool_belong(&my_pool, obj++));
	}

	do_free(&my_pool, objects, MY_POOL_SZ);
}

TEST_CASE("Only objects from pool may belong to it") {
	int i;
	void *obj, *obj_end, *not_obj;

	do_alloc(&my_pool, objects, MY_POOL_SZ);

	for (i = 0; i < MY_POOL_SZ; ++i) {
		obj = objects[i];
		test_assert_true(pool_belong(&my_pool, obj));
		obj_end = obj + my_pool.obj_size;
		for (not_obj = obj + 1; not_obj < obj_end; ++not_obj) {
			test_assert_false(pool_belong(&my_pool, not_obj));
		}
	}

	do_free(&my_pool, objects, MY_POOL_SZ);
}


static void do_alloc(struct pool *pool, struct object *objects[], int nr) {
	size_t i;
	for (i = 0; i < nr; ++i) {
		test_assert_not_null(
				(objects[i] = (struct object *) pool_alloc(pool)));
	}
}

static void do_free(struct pool *pool, struct object *objects[], int nr) {
	struct object *object;
	size_t i;
	for (i = 0; i < nr; ++i) {
		if ((object = objects[i])) {
			pool_free(pool, object);
		}
	}
}

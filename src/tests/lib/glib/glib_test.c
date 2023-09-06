#include <glib.h> 
#include <gmodule.h>
#include <glib-object.h>
#include <glib-unix.h>
#include <gio/gio.h>
#include <string.h>
#include <strings.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("lib/glib_test");

TEST_CASE("test g_array") {
	int n = 10;
	GArray *arr = g_array_new(false, false, sizeof(int));
	for (int i = 0; i < n; i++) {
		g_array_append_val(arr, i);
	}
	for (int i = 0; i < n; i++) {
		test_assert_equal(g_array_index(arr, int, i), i);
	}
	g_array_free(arr, true);
}

TEST_CASE("test g_path_get_dirname") {
	struct {
		gchar *path;
		gchar *expected;
	} dirnames[] = {
	    { "/", "/" },
    	{ "////", "/" },
	    { ".////", "." },
    	{ ".", "." },
	    { "..", "." },
    	{ "../", ".." },
	    { "..////", ".." },
    	{ "", "." },
	    { "a/b", "a" },
    	{ "a/b/", "a/b" },
	    { "c///", "c" },
    	{ "/a/b", "/a" },
	    { "/a/b/", "/a/b" },
	};
	int n = sizeof(dirnames) / sizeof(dirnames[0]);

	for (int i = 0; i < n; i++) {
		gchar *actual = g_path_get_dirname(dirnames[i].path);
		test_assert_equal(strcmp(actual, dirnames[i].expected), 0);
		g_free(actual);
	}
}

TEST_CASE("test env") {
	gboolean res;
	const gchar *actual;
	gchar *var = "TEST_G_VAR";
	gchar *val1 = "1";
	gchar *val2 = "2";
	
	actual = g_getenv(var);
	test_assert_null(actual);

	res = g_setenv(var, val1, true);
	test_assert_true(res);

	actual = g_getenv(var);
	test_assert_equal(strcmp(actual, val1), 0);


	res = g_setenv(var, val2, false);
	test_assert_true(res);

	actual = g_getenv(var);
	test_assert_equal(strcmp(actual, val1), 0);


	res = g_setenv(var, val2, true);
	test_assert_true(res);

	actual = g_getenv(var);
	test_assert_equal(strcmp(actual, val2), 0);

	g_unsetenv(var);
	actual = g_getenv(var);
	test_assert_null(actual);
}

TEST_CASE("test g_list") {
	gint n = 10;
	GList *list = NULL;
	
	for (gint i = 0; i < n; i++) {
		list = g_list_append(list, GINT_TO_POINTER(i));
	}
	test_assert_equal(g_list_length(list), n);

	for (gint i = 0; i < n; i++) {
		test_assert_equal(g_list_index(list, GINT_TO_POINTER(i)), i);
		test_assert_equal((gint)g_list_nth(list, i)->data, i);
	}
	list = g_list_reverse(list);

	for (gint i = 0; i < n; i++) {
		gint a = g_list_index(list, GINT_TO_POINTER(i));
		test_assert_equal(a, n-i-1);
		test_assert_equal((gint)g_list_nth(list, i)->data, n-i-1);
	}
	list = g_list_prepend(list, GINT_TO_POINTER(100));
	test_assert_equal((gint)g_list_first(list)->data, 100);
	g_list_free(list);
	list = NULL;
}

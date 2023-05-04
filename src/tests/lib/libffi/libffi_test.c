#include <stdio.h>
#include <ffi.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("lib/ffi_test");

static int return_int(int i) {
	return i + 1;
}

TEST_CASE("test the call return int") {
	ffi_cif cif;
	ffi_type *args[1];
	void *values[1];
	ffi_arg rc;
	
	int actual;
	args[0] = &ffi_type_sint;
	values[0] = &actual;

	test_assert_equal(ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 1, &ffi_type_sint, args), FFI_OK);
	for (actual = INT_MIN; actual < INT_MIN + 10; actual++) {
		ffi_call(&cif, FFI_FN(return_int), &rc, values);
		test_assert_equal((int)rc, actual + 1);
	}
	for (actual = -10; actual < 10; actual++) {
		ffi_call(&cif, FFI_FN(return_int), &rc, values);
		test_assert_equal((int)rc, actual + 1);
	}
	for (actual = INT_MAX - 10; actual < INT_MAX; actual++) {
		ffi_call(&cif, FFI_FN(return_int), &rc, values);
		test_assert_equal((int)rc, actual + 1);
	}
}


static char return_string() {
	return 't';
}

TEST_CASE("test the call return char") {
	ffi_cif cif;
	ffi_type *args[0];
	void *values[0];
	ffi_arg rc;

	test_assert_equal(ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 0, &ffi_type_schar, args), FFI_OK);

	ffi_call(&cif, FFI_FN(return_string), &rc, values);
	test_assert_equal(rc, 't');
}


struct test_struct {
	char ch;
	int i;
	float f;
};

static struct test_struct return_struct(struct test_struct s1) {
	s1.ch++;
	s1.i += 2;
	s1.f += 3;
	return s1;
}

TEST_CASE("test the call return struct") {
	ffi_cif cif;
	ffi_type *args[1];
	void *values[1];

	ffi_type struct_type;
	ffi_type *type_elements[4];
	
	struct_type.size = 0;
	struct_type.alignment = 0;
	struct_type.type = FFI_TYPE_STRUCT;
	struct_type.elements = type_elements;
	type_elements[0] = &ffi_type_schar;
	type_elements[1] = &ffi_type_sint;
	type_elements[2] = &ffi_type_float;
	type_elements[3] = NULL;

	struct test_struct struct_arg;
	args[0] = &struct_type;
	values[0] = &struct_arg;
	test_assert_equal(ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 1, &struct_type, args), FFI_OK);

	struct_arg.ch = 97;
	struct_arg.i = 42;
	struct_arg.f = 0;
	
	struct test_struct result;
	ffi_call(&cif, FFI_FN(return_struct), &result, values);

	test_assert_equal(result.ch, 98);
	test_assert_equal(result.i, 44);
	test_assert_equal(result.f, 3);
}

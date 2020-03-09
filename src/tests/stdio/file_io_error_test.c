/**
 * @file
 * @brief Tests for feof(), ferror() and clearerr() in compat/libc/stdio/
 *
 * @author Suraj Upadhyay
 * @date 08.03.2020
 */

#include <stdio.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("stdio/feof ferror clearerr test");

#define TEST_FILE_NAME "/file_io_error_test.txt" 
#define ERROR_MESSAGE(str) "Error encountered : "str 

TEST_CASE("feof and clearerr should set and clear EOF flag") {
    FILE *f;

    if (NULL == (f = fopen(TEST_FILE_NAME,"r"))) {
        test_fail(ERROR_MESSAGE("Cannot open file"));
    }

    test_assert_equal(feof(f), 0);
    test_assert_equal(fgetc(f), 'a');
    test_assert_equal(feof(f), 0);
    test_assert_equal(fgetc(f), EOF);
    test_assert_not_zero(feof(f));

    clearerr(f);

    test_assert_zero(feof(f));

    fclose(f);
}

TEST_CASE("ferror and clearerr should set and clear ERR flag") {
    FILE *f;

    if (NULL == (f = fopen(TEST_FILE_NAME, "r"))) {
        test_fail(ERROR_MESSAGE("Cannot open file mode : r"));
    }

    test_assert_zero(ferror(f));
    test_assert_equal(fputc('a', f), EOF);
    test_assert_not_zero(ferror(f));

    clearerr(f);

    test_assert_zero(ferror(f));

    if (NULL == (f = fopen(TEST_FILE_NAME, "w"))) {
        test_fail(ERROR_MESSAGE("Cannot open file mode : w"));
    }

    test_assert_zero(ferror(f));
    test_assert_equal(fgetc(f), EOF);
    test_assert_not_zero(ferror(f));

    clearerr(f);
    
    test_assert_zero(ferror(f));

    fclose(f);
}

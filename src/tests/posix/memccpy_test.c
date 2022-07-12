/**
 * @file
 * @brief Tests for memccpy function
 * 
 * @author Giannis Zapantis  (github name: tech-gian)
 * @date 18/3/2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <embox/test.h>


EMBOX_TEST_SUITE("memccpy");


/* TEST CASES for testing memccpy
 * on strings
 */


TEST_CASE("empty static allocated string with character off the string") {
    
    char original[] = "To be copied";
    char tested[128];

    void* test;

    test = memccpy(tested, original, 'w', strlen(original)+1);
    test_assert_zero(strcmp(tested, original));
    test_assert_null(test);
}


TEST_CASE("non-empty static allocated string with character off the string") {

    char original[] = "To be copied";
    char tested[] = "Before copying";

    void* test;

    test = memccpy(tested, original, 'w', strlen(original)+1);
    test_assert_zero(strcmp(tested, original)); 
    test_assert_null(test);
}


TEST_CASE("non-empty static allocated string with character on the string") {

    char original[] = "To be copied";
    char tested[] = "Before copy";

    void* test;

    test = memccpy(tested, original, 'c', strlen(original)+1);
    test_assert_zero(strcmp(tested, "To be  copy"));
    test_assert_not_null(test);
}


TEST_CASE("empty static allocated string with character on the string") {

    char original[] = "To be copied";
    char tested[128];
    char c = 'c';

    void* test;
    test = memccpy(tested, original, c, strlen(original)+1);

    char* cPos = strchr(original, c);
    tested[cPos - original] = '\0';

    test_assert_zero(strcmp(tested, "To be "));
    test_assert_not_null(test);
}


TEST_CASE("dynamic allocated string with character off the string") {

    char original[] = "To be copied";
    char* tested;

    tested = malloc(128*sizeof(char));

    void* test;

    test = memccpy(tested, original, 'w', strlen(original)+1);
    test_assert_zero(strcmp(tested, original));
    test_assert_null(test);

    free(tested);
}


TEST_CASE("dynamic allocated string with character on the string") {

    char original[] = "To be copied";
    char* tested;
    char c = 'c';

    tested = malloc(128*sizeof(char));

    void* test;
    test = memccpy(tested, original, c, strlen(original)+1);

    char* cPos = strchr(original, c);
    tested[cPos - original] = '\0';

    test_assert_zero(strcmp(tested, "To be "));
    test_assert_not_null(test);

    free(tested);
}


/* TEST CASES for testing memccpy
 * on ints
 */


TEST_CASE("empty static allocated array with number on array") {

    int original[] = {1, 2, 3, 4};
    int tested[4];
    int stoping_c = 3;

    void* test;
    int i;

    test = memccpy(tested, original, stoping_c, sizeof(original));
    for (i = 0 ; i < sizeof(original) / sizeof(int) ; i++) {
        if  (original[i] == stoping_c) {
            break;
        }
        test_assert_equal(tested[i], original[i]);
    }
    test_assert_not_null(test);
}


TEST_CASE("non-empty static allocated array with number on array") {

    int original[] = {1, 2, 3, 4};
    int tested[] = {5, 6, 7, 8};
    int stoping_c = 3;
 
    void* test;
    int i;

    test = memccpy(tested, original, stoping_c, sizeof(original));
    for (i = 0 ; i < sizeof(original) / sizeof(int) ; i++) {
        if  (original[i] == stoping_c) {
            break;
        }
        test_assert_equal(tested[i], original[i]);
    }
    test_assert_equal(tested[2], 7); test_assert_equal(tested[3], 8);
    test_assert_not_null(test);
}


TEST_CASE("dynamic allocated array with number on array") {

    int original[] = {1, 2, 3, 4};
    int* tested;
    tested = malloc(4*sizeof(int));
    int stoping_c = 3;

    void* test;
    int i;

    test = memccpy(tested, original, stoping_c, sizeof(original));
    for (i = 0 ; i < sizeof(original) / sizeof(int) ; i++) {
        if  (original[i] == stoping_c) {
            break;
        }
        test_assert_equal(tested[i], original[i]);
    }
    test_assert_not_null(test);

    free(tested);
}

/**
 * @file
 *
 * @date Apr 26, 2020
 * @author: Nakul Shahdadpuri
 */
#include <string.h>
#include <stdlib.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test suite for memrchr()");

TEST_CASE(){

	char str = "ABCDEFGH"
	char *ps = memrchr(str,'F',strlen(str)); /*test case*/

	test_assert_not_null(ps);	/*checking if a character is found*/
	test_assert_zero(strcmp(ps,"FGH"))		/*checking if the right character is found*/
}

TEST_CASE(){

	char str = "12345678"
	char *ps = memrchr(str,'4',strlen(str)); /*test case*/

	test_assert_not_null(ps);	/*checking if a character is found*/
	test_assert_zero(strcmp(ps,"45678"))		/*checking if the right character is found*/
}

TEST_CASE(){

	char str = "12345678"
	char *ps = memrchr(str,'a',strlen(str)); /*test case*/

	test_assert_null(ps);	/*checking if a character is not found*/
}

TEST_CASE(){
	char str = ""
	char *ps = memrchr(str,'a',strlen(str)); /*test case*/
	
	test_assert_null(ps);	/*checking if a character is not found*/
}
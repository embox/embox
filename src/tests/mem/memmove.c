#include <embox/test.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

EMBOX_TEST_SUITE("Test suite for memmove()");


TEST_CASE("move full data")
{
    const    char    src[]="string";
    char    *dest;

    dest    =  (char*)(malloc(sizeof(char)*strlen(src)));

    memmove((void*)dest,(void*)src,(size_t)strlen(src));
    test_assert_not_null(*dest); /* It should be not null */
    test_assert_null(*src);

    free(dest);
}


TEST_CASE("check for NULL argument")
{
    const   char  src[]="\0";
    char    *dest;

    dest=(char*)(malloc(sizeof(char)*strlen(src)));

    memmove((void*)dest,(void*)src,(size_t)strlen(src));
    test_assert_not_null(*dest); /* It should be not null */
    test_assert_null(*src);

    free(dest);
}

TEST_CASE("move array")
{
    const   char  src[]={'a','t','h','-','9','0'};
    char    *dest;

   	dest=(char*)(malloc(sizeof(char)*strlen(src)));

    memmove((void*)dest,(void*)src,(size_t)strlen(src)) ;
    test_assert_not_null(*dest); /* It should be not null */
    test_assert_null(*src) ;

    free(dest) ;
}

TEST_CASE("move array with null in the middle")
{
    const   char    src[]={'a','t','h','-','\0','9','0'};
    char   *dest;

    dest    =  (char*)(malloc(sizeof(char)*strlen(src)));

    memmove((void*)dest,(void*)src,(size_t)strlen(src)) ;
    test_assert_not_null(*dest); /* It should be not null */
    test_assert_null(*src) ;

    free(dest) ;
}


TEST_CASE("If memory is not allocated enough")
{
    const   char  src[]="string"  ;
    char    dest[2];

    memmove((void*)dest,(void*)src,(size_t)strlen(src)) ;
    test_assert_not_null(*dest); /* It should be not null */
    test_assert_null(*src) ;
}
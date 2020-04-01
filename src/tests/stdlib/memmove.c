#include <embox/test.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

EMBOX_TEST_SUITE("Test suite for memmove() function");


TEST_CASE("move full data")
{
    const    char    src[]="string";
    char    dest[sizeof(src)+1];

    test_assert_not_null(dest) ;
    memmove((void*)dest,(void*)src,(size_t)strlen(src));
    test_assert_equal(*dest , *src) ;
}


TEST_CASE("check for NULL argument")
{
    const   char  src[]="\0";
    char    dest[sizeof(src)+1];

    test_assert_not_null(dest) ;
    memmove((void*)dest,(void*)src,(size_t)strlen(src));
    test_assert_equal(*dest , *src) ;
}

TEST_CASE("move array")
{
    const   char  src[]={'a','t','h','-','9','0'};
    char    dest[sizeof(src)+1];

    test_assert_not_null(dest) ;
    memmove((void*)dest,(void*)src,(size_t)strlen(src));
    test_assert_equal(*dest , *src) ;
}

TEST_CASE("move array with null in the middle")
{
    const   char    src[]={'a','t','h','-','\0','9','0'};
    char   dest[sizeof(src)+1] ;

    test_assert_not_null(dest) ;
    memmove((void*)dest,(void*)src,(size_t)strlen(src));
    test_assert_equal(*dest , *src) ;
}


TEST_CASE("If memory is not allocated enough")
{
    const   char  src[]="string"  ;
    char    dest[2];

    test_assert_not_null(dest) ;
    memmove((void*)dest,(void*)src,(size_t)strlen(src));
    test_assert_equal(*dest , *src) ;
}

#include <embox/test.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

EMBOX_TEST_SUITE("Test suite for memmove function");

TEST_CASE("move full data")
{
    char src[] = "string" ;
    char dest[strlen(src)+1] ;

    memmove(dest,src,strlen(src));
    test_assert_equal(*dest,*src);
}

TEST_CASE("Check for null argument")
{
    char src[] = "\0" ;
    char dest[strlen(src)+1] ;

    memmove(dest,src,strlen(src));
    test_assert_equal(*dest,*src);
}

TEST_CASE("move array")
{
    char src[] = {'s','t','r','i','n','g'} ;
    char dest[strlen(src)+1] ;

    memmove(dest,src,strlen(src));
    test_assert_equal(*dest,*src);
}

TEST_CASE("Move array with null value in the middle")
{
    char src[] = {'s','t','r','\0','i','n','g'} ;
    char dest[strlen(src)+1] ;

    memmove(dest,src,strlen(src));
    test_assert_equal(*dest,*src);
}

TEST_CASE("If memory is not allocated enough")
{
    char src[] = {'s','t','r','\0','i','n','g'} ;
    char dest[2] ;

    memmove(dest,src,strlen(src));
    test_assert_equal(*dest,*src);
}
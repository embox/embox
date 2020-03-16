#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "acutest.h"
#include "embox_memccpy.h"



//function for testing strings
void memccpy_test_strings(void) {
//original string
    char t1[] = "To be copied";
    void* test;

//empty static allocated string
//with character off the string
    char t2[128];
    test = embox_memccpy(t2, t1, 'w', strlen(t1)+1);
    TEST_CHECK(strcmp(t2, t1)==0 && test==NULL);


//non-empty static allocated string
//with character off the string
    char t3[] = "Before copy";
    test = embox_memccpy(t3, t1, 'w', strlen(t1)+1);
    TEST_CHECK(strcmp(t3, t1)==0 && test==NULL);


//non-empty static allocated string
//with character on the string
    char t5[] = "Before copy";
    test = embox_memccpy(t5, t1, 'c', strlen(t1)+1);
    TEST_CHECK(strcmp(t5, "To be  copy")==0 && test!=NULL);


//empty static allocated string
//with character on the string
    char t6[128];
    test = embox_memccpy(t6, t1, 'c', strlen(t1)+1);
    TEST_CHECK(strcmp(t6, "To be ")==0 && test!=NULL);


//dynamic allocated string
//with character off the string
    char* t4 = malloc(128*sizeof(char));
    test = embox_memccpy(t4, t1, 'w', strlen(t1)+1);
    TEST_CHECK(strcmp(t4, t1)==0 && test==NULL);


//dynamic allocated string
//with character on the string
    t4 = malloc(128*sizeof(char));
    test = embox_memccpy(t4, t1, 'c', strlen(t1)+1);
    TEST_CHECK(strcmp(t4, "To be ")==0 && test!=NULL);
}



//function for testing ints
void memccpy_test_ints(void) {
//original array
    int t1[] = {1, 2, 3, 4};

//empty static allocated array
    int t2[4];

//non-empty static allocated array
    int t3[] = {5, 6, 7, 8};

//dynamic allocated array
    int* t4 = malloc(4*sizeof(int));

    void* test;


//with number off array
    test = embox_memccpy(t2, t1, 0, sizeof(t1));
    for (int i=0 ; i<sizeof(t1)/sizeof(int) ; i++) {
        TEST_CHECK(t1[i]==t2[i]);
    }    
    TEST_CHECK(test==NULL);


//with number off array
    test = embox_memccpy(t3, t1, 0, sizeof(t1));
    for (int i=0 ; i<sizeof(t1)/sizeof(int) ; i++) {
        TEST_CHECK(t1[i]==t3[i]);
    }    
    TEST_CHECK(test==NULL);


//with number off array
    test = embox_memccpy(t4, t1, 0, sizeof(t1));
    for (int i=0 ; i<sizeof(t1)/sizeof(int) ; i++) {
        TEST_CHECK(t4[i]==t1[i]);
    }    
    TEST_CHECK(test==NULL);


//empty static allocated array
    int t5[4];
//with number on array
    test = embox_memccpy(t5, t1, 3, sizeof(t1));
    for (int i=0 ; i<sizeof(t1)/sizeof(int) ; i++) {
        if (t1[i]==3) break;
        TEST_CHECK(t1[i]==t5[i]);
    }
    TEST_CHECK(test!=NULL);


//non-empty static allocated array
    int t6[] = {5, 6, 7, 8};
//with number on array
    test = embox_memccpy(t6, t1, 3, sizeof(t1));
    for (int i=0 ; i<sizeof(t1)/sizeof(int) ; i++) {
        if (t1[i]==3) break;
        TEST_CHECK(t1[i]==t6[i]);
    }
    TEST_CHECK(t6[2]==7); TEST_CHECK(t6[3]==8);
    TEST_CHECK(test!=NULL);


//dynamic allocated array
    int* t7 = malloc(4*sizeof(int));
//with number on array
    test = embox_memccpy(t7, t1, 3, sizeof(t1));
    for (int i=0 ; i<sizeof(t1)/sizeof(int) ; i++) {
        if (t1[i]==3) break;
        TEST_CHECK(t1[i]==t7[i]);
    }
    TEST_CHECK(test!=NULL);

}



//      TEST_LIST
TEST_LIST = {
    { "memccpy_test_strings", memccpy_test_strings},
    { "memccpy_test_ints", memccpy_test_ints},

    {NULL, NULL}    //ending with NULL list
};
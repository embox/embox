#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "embox_memccpy.h"



void* embox_memccpy(void* dest, const void* src, int c, size_t n) {

// If overlap happens the results
// will be undefined


// if there is already data after c
// then that data stays the same
// if there isn't we don't change
// anything


//copying pointer dest and src
//into one byte (char) pointers

//so we can work for each byte separately
    char* temp_dest = (char*) dest;
    char* temp_src = (char*) src;
    size_t pos = n;

    char* ar1[n];

//getting each byte for c
    char temp_c[4];
    temp_c[0] = (c>>24) & 0xFF;
    temp_c[1] = (c>>16) & 0xFF;
    temp_c[2] = (c>>8) & 0xFF;
    temp_c[3] = c & 0xFF;


//if c!=0 then we check the last byte
//if there is in src
if (c!=0) {
    for (size_t i=0 ; i<n ; i++) {
        if (temp_src[i]==temp_c[3] && temp_c[3]!=0) {
            pos = i;
            break;
        }
    }
}
else {
//if c==0 we check every byte to be
//the same (4 bytes of int)
    for (size_t i=0 ; i<n ; i++) {
        if (i<n-4) {
            if (temp_src[i]==temp_c[0] && temp_src[i+1]==temp_c[1] && temp_src[i+2]==temp_c[2] && temp_src[i+3]==temp_c[3]) {
                pos = i;
                break;
            }
        }
    }
}

//getting the pointers from src
//into an array of pointers
    for (size_t i=0 ; i<n ; i++) {
        ar1[i] = &(temp_src[i]);
    }


//copying the value from src
//to dest until c found
    for (size_t i=0 ; i<n ; i++) {
        if (i<pos) {
            temp_dest[i] = *ar1[i];
        }
        else return dest+i;
    }

    return NULL;
//return NULL if c wasn't found in src
//else the next pointer of dest
}
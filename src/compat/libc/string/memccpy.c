/**
 *  @file
 *  @brief Implementation of #memccpy() function.
 *
 *  @date 18/3/20
 *  @author Giannis Zapantis (github name: tech-gian)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void* memccpy(void* dest, const void* src, int c, size_t n) {

/* If overlap happens the results
 * will be undefined.
 */

/* Getting dest and srt into byte
 * format.
 */
    char* temp_dest;
    char* temp_src;
    
    temp_dest = (char*) dest;
    temp_src = (char*) src;

    size_t i;


/* copying the value from src
 * to dest until c found.
 */
    for (i = 0 ; i < n ; i++) {
        if (temp_src[i] == c) {
            return &temp_dest[i];
        }
        temp_dest[i] = temp_src[i];
    }

    return NULL;

/* return NULL if c wasn't found in src.
 *
 * else the next pointer of dest.
 */
}

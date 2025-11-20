#pragma once

/* Очень минимальная заглушка stdlib.h под Embox,
 * чтобы удовлетворить <cstdlib> из libstdc++.
 */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void *malloc(size_t);
void  free(void *);
void *realloc(void *, size_t);
void *calloc(size_t, size_t);
void  abort(void) __attribute__((__noreturn__));

#ifdef __cplusplus
}
#endif

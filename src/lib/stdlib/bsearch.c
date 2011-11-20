/**
 * @file
 * @brief Binary search algorithm.
 *
 * @see stdlib.h
 *
 * @date 19.11.11
 * @author Avdyukhin Dmitry
 */

#include <stdlib.h>

void *bsearch(const void *key, const void *base,
              size_t nmemb, size_t size,
              int (*compar)(const void *, const void *)) {
	void *left = (void *)base,
		*right = (void *)base + size * nmemb,
		*mid;
	while (left + size < right) {
		mid = left + ((right - left) / (size << 1) * size);
		if (compar(key, mid) < 0) {
			right = mid;
		} else {
			left = mid;
		}
	}
	if (compar(left, key) == 0) {
		return left;
	} else {
		return NULL;
	}
}



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
#include <stddef.h>

void *upper_bound(const void *key, const void *base,
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
	return right;
}

void *lower_bound(const void *key, const void *base,
              size_t nmemb, size_t size,
              int (*compar)(const void *, const void *)) {
	void *left = (void *)base - size,
		*right = (void *)base + size * (nmemb - 1),
		*mid;
	while (left + size < right) {
		mid = left + ((right - left) / (size << 1) * size);
		if (compar(key, mid) <= 0) {
			right = mid;
		} else {
			left = mid;
		}
	}
	return right;
}

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

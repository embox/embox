/**
 * @file
 * @brief Standart quick sort algorithm.
 *
 * @see stdlib.h
 *
 * @date 19.11.11
 * @author Avdyukhin Dmitry
 */

#include <stdlib.h>
#include <string.h>
#include <alloca.h>

/**
 * Swap two elements of specified size.
 */
static inline void swap(void *fst, void *snd, size_t size) {
	void *temp = alloca(size);

	memcpy(temp, snd, size);
	memcpy(snd, fst, size);
	memcpy(fst, temp, size);
}

void qsort(void *vbase, size_t nmemb, size_t size,
		int(*compar)(const void *, const void *)) {
	char *base = (char *) vbase;

	if (nmemb < 4) {
		if (nmemb == 2) {
			if (compar(base + size, base) < 0) {
				swap(base, base + size, size);
			}
		} else if (nmemb == 3) {
			if (compar(base + size, base) < 0) {
				swap(base, base + size, size);
			}
			if (compar(base + (size << 1), base + size) < 0) {
				swap(base + size, base + (size << 1), size);
				if (compar(base + size, base) < 0) {
					swap(base, base + size, size);
				}
			}
		}
		return;
	} else {
		char *pos = (rand() % nmemb) * size + base;
		char *key = alloca(size);
		char *i = base, *j = base + (size * (nmemb - 1));

		memcpy(key, pos, size);

		while (i <= j) {
			while (compar(i, key) < 0) {
				i += size;
			}
			while (compar(key, j) < 0) {
				j -= size;
			}
			if (i <= j) {
				swap(i, j, size);
				i += size;
				j -= size;
			}
		}
		if (j > base) {
			qsort(base, (j - base) / size + 1, size, compar);
		}
		if (i < base + (nmemb - 1) * size) {
			qsort(i, nmemb - (i - base) / size, size, compar);
		}

	}
}

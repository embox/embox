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

/**
 * Swap two elements of specified size.
 */
static inline void swap(void *fst, void *snd, size_t size) {
	char temp[size];
	memcpy((void *)temp, snd, size);
	memcpy(snd, fst, size);
	memcpy(fst, (void *)temp, size);
}

void qsort(void *base, size_t nmemb, size_t size,
		int(*compar)(const void *, const void *)) {
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
		void *pos = (rand() % nmemb) * size + base;
		char key[size];
		void *i = base, *j = base + (size * (nmemb - 1));
	    memcpy((void *)key, pos, size);
		while (i <= j) {
			while (compar(i, (void *)key) < 0) {
				i += size;
			}
			while (compar((void *)key, j) < 0) {
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



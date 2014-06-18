/**
 * @file
 * @brief Implementation of #strcspn() function.
 *
 * @date 10.11.11
 * @author Nikolay Korotkiy
 */

#include <string.h>

size_t strcspn(const char *s, const char *reject) {
	size_t count = 0;

	while (*s != '\0') {
		if (strchr(reject, *s++) == NULL) {
			++count;
		} else {
			return count;
		}
	}
	return count;
}

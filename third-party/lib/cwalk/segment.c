/**
 * MIT License
 *
 * Copyright (c) 2020 Leonard Iklé
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "cwalk_priv.h"

static const char *cwk_path_find_previous_stop(const char *begin,
    const char *c) {
	// We just move back until we find a separator or reach the beginning of the
	// path, which will be our previous "stop".
	while (c > begin && cwk_path_is_relative(c)) {
		--c;
	}

	// Return the pointer to the previous stop. We have to return the first
	// character after the separator, not on the separator itself.
	if (cwk_path_is_absolute(c)) {
		return c + 1;
	}
	else {
		return c;
	}
}

bool cwk_path_get_first_segment(const char *path, struct cwk_segment *segment) {
	size_t length;
	const char *segments;

	// We skip the root since that's not part of the first segment. The root is
	// treated as a separate entity.
	cwk_path_get_root(path, &length);
	segments = path + length;

	// Now, after we skipped the root we can continue and find the actual segment
	// content.
	return cwk_path_get_first_segment_without_root(path, segments, segment);
}

bool cwk_path_get_last_segment(const char *path, struct cwk_segment *segment) {
	// We first grab the first segment. This might be our last segment as well,
	// but we don't know yet. There is no last segment if there is no first
	// segment, so we return false in that case.
	if (!cwk_path_get_first_segment(path, segment)) {
		return false;
	}

	// Now we find our last segment. The segment struct of the caller
	// will contain the last segment, since the function we call here will not
	// change the segment struct when it reaches the end.
	while (cwk_path_get_next_segment(segment)) {
		// We just loop until there is no other segment left.
	}

	return true;
}

bool cwk_path_get_next_segment(struct cwk_segment *segment) {
	const char *c;

	// First we jump to the end of the previous segment. The first character must
	// be either a '\0' or a separator.
	c = segment->begin + segment->size;
	if (*c == '\0') {
		return false;
	}

	// Now we skip all separator until we reach something else. We are not yet
	// guaranteed to have a segment, since the string could just end afterwards.
	assert(cwk_path_is_absolute(c));
	do {
		++c;
	} while (cwk_path_is_absolute(c));

	// If the string ends here, we can safely assume that there is no other
	// segment after this one.
	if (*c == '\0') {
		return false;
	}

	// Now we are safe to assume there is a segment. We store the beginning of
	// this segment in the segment struct of the caller.
	segment->begin = c;

	// And now determine the size of this segment, and store it in the struct of
	// the caller as well.
	c = cwk_path_find_next_stop(c);
	segment->end = c;
	segment->size = (size_t)(c - segment->begin);

	// Tell the caller that we found a segment.
	return true;
}

bool cwk_path_get_previous_segment(struct cwk_segment *segment) {
	const char *c;

	// The current position might point to the first character of the path, which
	// means there are no previous segments available.
	c = segment->begin;
	if (c <= segment->segments) {
		return false;
	}

	// We move towards the beginning of the path until we either reached the
	// beginning or the character is no separator anymore.
	do {
		--c;
		if (c < segment->segments) {
			// So we reached the beginning here and there is no segment. So we return
			// false and don't change the segment structure submitted by the caller.
			return false;
		}
	} while (cwk_path_is_absolute(c));

	// We are guaranteed now that there is another segment, since we moved before
	// the previous separator and did not reach the segment path beginning.
	segment->end = c + 1;
	segment->begin = cwk_path_find_previous_stop(segment->segments, c);
	segment->size = (size_t)(segment->end - segment->begin);

	return true;
}

enum cwk_segment_type cwk_path_get_segment_type(
    const struct cwk_segment *segment) {
	// We just make a string comparison with the segment contents and return the
	// appropriate type.
	if (strncmp(segment->begin, ".", segment->size) == 0) {
		return CWK_CURRENT;
	}
	else if (strncmp(segment->begin, "..", segment->size) == 0) {
		return CWK_BACK;
	}

	return CWK_NORMAL;
}

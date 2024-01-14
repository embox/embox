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

#include <stdbool.h>
#include <stddef.h>

#include "cwalk_priv.h"

size_t cwk_path_get_intersection(const char *path_base,
    const char *path_other) {
	struct cwk_segment_joined base, other;
	const char *paths_base[2], *paths_other[2];
	const char *end;
	size_t base_root_length, other_root_length;
	bool absolute;

	// We first compare the two roots. We just return zero if they are not equal.
	// This will also happen to return zero if the paths are mixed relative and
	// absolute.
	cwk_path_get_root(path_base, &base_root_length);
	cwk_path_get_root(path_other, &other_root_length);
	if (!cwk_path_is_string_equal(path_base, path_other, base_root_length,
	        other_root_length)) {
		return 0;
	}

	// Configure our paths. We just have a single path in here for now.
	paths_base[0] = path_base;
	paths_base[1] = NULL;
	paths_other[0] = path_other;
	paths_other[1] = NULL;

	// So we get the first segment of both paths. If one of those paths don't have
	// any segment, we will return 0.
	if (!cwk_path_get_first_segment_joined(paths_base, &base)
	    || !cwk_path_get_first_segment_joined(paths_other, &other)) {
		return base_root_length;
	}

	// We now determine whether the path is absolute or not. This is required
	// because if will ignore removed segments, and this behaves differently if
	// the path is absolute. However, we only need to check the base path because
	// we are guaranteed that both paths are either relative or absolute.
	absolute = cwk_path_is_absolute(path_base);

	// We must keep track of the end of the previous segment. Initially, this is
	// set to the beginning of the path. This means that 0 is returned if the
	// first segment is not equal.
	end = path_base + base_root_length;

	// Now we loop over both segments until one of them reaches the end or their
	// contents are not equal.
	do {
		// We skip all segments which will be removed in each path, since we want to
		// know about the true path.
		if (!cwk_path_segment_joined_skip_invisible(&base, absolute)
		    || !cwk_path_segment_joined_skip_invisible(&other, absolute)) {
			break;
		}

		if (!cwk_path_is_string_equal(base.segment.begin, other.segment.begin,
		        base.segment.size, other.segment.size)) {
			// So the content of those two segments are not equal. We will return the
			// size up to the beginning.
			return (size_t)(end - path_base);
		}

		// Remember the end of the previous segment before we go to the next one.
		end = base.segment.end;
	} while (cwk_path_get_next_segment_joined(&base)
	         && cwk_path_get_next_segment_joined(&other));

	// Now we calculate the length up to the last point where our paths pointed to
	// the same place.
	return (size_t)(end - path_base);
}

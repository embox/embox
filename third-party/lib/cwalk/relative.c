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

static void cwk_path_skip_segments_until_diverge(struct cwk_segment_joined *bsj,
    struct cwk_segment_joined *osj, bool absolute, bool *base_available,
    bool *other_available) {
	// Now looping over all segments until they start to diverge. A path may
	// diverge if two segments are not equal or if one path reaches the end.
	do {
		// Check whether there is anything available after we skip everything which
		// is invisible. We do that for both paths, since we want to let the caller
		// know which path has some trailing segments after they diverge.
		*base_available = cwk_path_segment_joined_skip_invisible(bsj, absolute);
		*other_available = cwk_path_segment_joined_skip_invisible(osj,
		    absolute);

		// We are done if one or both of those paths reached the end. They either
		// diverge or both reached the end - but in both cases we can not continue
		// here.
		if (!*base_available || !*other_available) {
			break;
		}

		// Compare the content of both segments. We are done if they are not equal,
		// since they diverge.
		if (!cwk_path_is_string_equal(bsj->segment.begin, osj->segment.begin,
		        bsj->segment.size, osj->segment.size)) {
			break;
		}

		// We keep going until one of those segments reached the end. The next
		// segment might be invisible, but we will check for that in the beginning
		// of the loop once again.
		*base_available = cwk_path_get_next_segment_joined(bsj);
		*other_available = cwk_path_get_next_segment_joined(osj);
	} while (*base_available && *other_available);
}

size_t cwk_path_get_relative(const char *base_directory, const char *path,
    char *buffer, size_t buffer_size) {
	size_t pos, base_root_length, path_root_length;
	bool absolute, base_available, other_available, has_output;
	const char *base_paths[2], *other_paths[2];
	struct cwk_segment_joined bsj, osj;

	pos = 0;

	// First we compare the roots of those two paths. If the roots are not equal
	// we can't continue, since there is no way to get a relative path from
	// different roots.
	cwk_path_get_root(base_directory, &base_root_length);
	cwk_path_get_root(path, &path_root_length);
	if (base_root_length != path_root_length
	    || !cwk_path_is_string_equal(base_directory, path, base_root_length,
	        path_root_length)) {
		cwk_path_terminate_output(buffer, buffer_size, pos);
		return pos;
	}

	// Verify whether this is an absolute path. We need to know that since we can
	// remove all back-segments if it is.
	absolute = cwk_path_is_absolute(base_directory);

	// Initialize our joined segments. This will allow us to use the internal
	// functions to skip until diverge and invisible. We only have one path in
	// them though.
	base_paths[0] = base_directory;
	base_paths[1] = NULL;
	other_paths[0] = path;
	other_paths[1] = NULL;
	cwk_path_get_first_segment_joined(base_paths, &bsj);
	cwk_path_get_first_segment_joined(other_paths, &osj);

	// Okay, now we skip until the segments diverge. We don't have anything to do
	// with the segments which are equal.
	cwk_path_skip_segments_until_diverge(&bsj, &osj, absolute, &base_available,
	    &other_available);

	// Assume there is no output until we have got some. We will need this
	// information later on to remove trailing slashes or alternatively output a
	// current-segment.
	has_output = false;

	// So if we still have some segments left in the base path we will now output
	// a back segment for all of them.
	if (base_available) {
		do {
			// Skip any invisible segment. We don't care about those and we don't need
			// to navigate back because of them.
			if (!cwk_path_segment_joined_skip_invisible(&bsj, absolute)) {
				break;
			}

			// Toggle the flag if we have output. We need to remember that, since we
			// want to remove the trailing slash.
			has_output = true;

			// Output the back segment and a separator. No need to worry about the
			// superfluous segment since it will be removed later on.
			pos += cwk_path_output_back(buffer, buffer_size, pos);
			pos += cwk_path_output_separator(buffer, buffer_size, pos);
		} while (cwk_path_get_next_segment_joined(&bsj));
	}

	// And if we have some segments available of the target path we will output
	// all of those.
	if (other_available) {
		do {
			// Again, skip any invisible segments since we don't need to navigate into
			// them.
			if (!cwk_path_segment_joined_skip_invisible(&osj, absolute)) {
				break;
			}

			// Toggle the flag if we have output. We need to remember that, since we
			// want to remove the trailing slash.
			has_output = true;

			// Output the current segment and a separator. No need to worry about the
			// superfluous segment since it will be removed later on.
			pos += cwk_path_output_sized(buffer, buffer_size, pos,
			    osj.segment.begin, osj.segment.size);
			pos += cwk_path_output_separator(buffer, buffer_size, pos);
		} while (cwk_path_get_next_segment_joined(&osj));
	}

	// If we have some output by now we will have to remove the trailing slash. We
	// simply do that by moving back one character. The terminate output function
	// will then place the '\0' on this position. Otherwise, if there is no
	// output, we will have to output a "current directory", since the target path
	// points to the base path.
	if (has_output) {
		--pos;
	}
	else {
		pos += cwk_path_output_current(buffer, buffer_size, pos);
	}

	// Finally, we can terminate the output - which means we place a '\0' at the
	// current position or at the end of the buffer.
	cwk_path_terminate_output(buffer, buffer_size, pos);

	return pos;
}

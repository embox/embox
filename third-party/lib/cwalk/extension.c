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
#include <string.h>

#include "cwalk_priv.h"

static size_t cwk_path_output_dot(char *buffer, size_t buffer_size,
    size_t position) {
	// We output a dot, which is a single character. This is used for extensions.
	return cwk_path_output_sized(buffer, buffer_size, position, ".", 1);
}

static size_t cwk_path_output(char *buffer, size_t buffer_size, size_t position,
    const char *str) {
	size_t length;

	// This just does a sized output internally, but first measuring the
	// null-terminated string.
	length = strlen(str);
	return cwk_path_output_sized(buffer, buffer_size, position, str, length);
}

bool cwk_path_get_extension(const char *path, const char **extension,
    size_t *length) {
	struct cwk_segment segment;
	const char *c;

	// We get the last segment of the path. The last segment will contain the
	// extension if there is any.
	if (!cwk_path_get_last_segment(path, &segment)) {
		return false;
	}

	// Now we search for a dot within the segment. If there is a dot, we consider
	// the rest of the segment the extension. We do this from the end towards the
	// beginning, since we want to find the last dot.
	for (c = segment.end; c >= segment.begin; --c) {
		if (*c == '.') {
			// Okay, we found an extension. We can stop looking now.
			*extension = c;
			*length = (size_t)(segment.end - c);
			return true;
		}
	}

	// We couldn't find any extension.
	return false;
}

bool cwk_path_has_extension(const char *path) {
	const char *extension;
	size_t length;

	// We just wrap the get_extension call which will then do the work for us.
	return cwk_path_get_extension(path, &extension, &length);
}

size_t cwk_path_change_extension(const char *path, const char *new_extension,
    char *buffer, size_t buffer_size) {
	struct cwk_segment segment;
	const char *c, *old_extension;
	size_t pos, root_size, trail_size, new_extension_size;

	// First we try to get the last segment. We may only have a root without any
	// segments, in which case we will create one.
	if (!cwk_path_get_last_segment(path, &segment)) {
		// So there is no segment in this path. First we grab the root and output
		// that. We are not going to modify the root in any way. If there is no
		// root, this will end up with a root size 0, and nothing will be written.
		cwk_path_get_root(path, &root_size);
		pos = cwk_path_output_sized(buffer, buffer_size, 0, path, root_size);

		// Add a dot if the submitted value doesn't have any.
		if (*new_extension != '.') {
			pos += cwk_path_output_dot(buffer, buffer_size, pos);
		}

		// And finally terminate the output and return the total size of the path.
		pos += cwk_path_output(buffer, buffer_size, pos, new_extension);
		cwk_path_terminate_output(buffer, buffer_size, pos);
		return pos;
	}

	// Now we seek the old extension in the last segment, which we will replace
	// with the new one. If there is no old extension, it will point to the end of
	// the segment.
	old_extension = segment.end;
	for (c = segment.begin; c < segment.end; ++c) {
		if (*c == '.') {
			old_extension = c;
		}
	}

	pos = cwk_path_output_sized(buffer, buffer_size, 0, segment.path,
	    (size_t)(old_extension - segment.path));

	// If the new extension starts with a dot, we will skip that dot. We always
	// output exactly one dot before the extension. If the extension contains
	// multiple dots, we will output those as part of the extension.
	if (*new_extension == '.') {
		++new_extension;
	}

	// We calculate the size of the new extension, including the dot, in order to
	// output the trail - which is any part of the path coming after the
	// extension. We must output this first, since the buffer may overlap with the
	// submitted path - and it would be overridden by longer extensions.
	new_extension_size = strlen(new_extension) + 1;
	trail_size = cwk_path_output(buffer, buffer_size, pos + new_extension_size,
	    segment.end);

	// Finally we output the dot and the new extension. The new extension itself
	// doesn't contain the dot anymore, so we must output that first.
	pos += cwk_path_output_dot(buffer, buffer_size, pos);
	pos += cwk_path_output(buffer, buffer_size, pos, new_extension);

	// Now we terminate the output with a null-terminating character, but before
	// we do that we must add the size of the trail to the position which we
	// output before.
	pos += trail_size;
	cwk_path_terminate_output(buffer, buffer_size, pos);

	// And the position is our output size now.
	return pos;
}

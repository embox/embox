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

#include <stddef.h>
#include <string.h>

#include "cwalk_priv.h"

size_t cwk_path_change_segment(struct cwk_segment *segment, const char *value,
    char *buffer, size_t buffer_size) {
	size_t pos, value_size, tail_size;

	// First we have to output the head, which is the whole string up to the
	// beginning of the segment. This part of the path will just stay the same.
	pos = cwk_path_output_sized(buffer, buffer_size, 0, segment->path,
	    (size_t)(segment->begin - segment->path));

	// In order to trip the submitted value, we will skip any separator at the
	// beginning of it and behave as if it was never there.
	while (cwk_path_is_absolute(value)) {
		++value;
	}

	// Now we determine the length of the value. In order to do that we first
	// locate the '\0'.
	value_size = 0;
	while (value[value_size]) {
		++value_size;
	}

	// Since we trim separators at the beginning and in the end of the value we
	// have to subtract from the size until there are either no more characters
	// left or the last character is no separator.
	while (value_size > 0 && cwk_path_is_absolute(&value[value_size - 1])) {
		--value_size;
	}

	// We also have to determine the tail size, which is the part of the string
	// following the current segment. This part will not change.
	tail_size = strlen(segment->end);

	// Now we output the tail. We have to do that, because if the buffer and the
	// source are overlapping we would override the tail if the value is
	// increasing in length.
	cwk_path_output_sized(buffer, buffer_size, pos + value_size, segment->end,
	    tail_size);

	// Finally we can output the value in the middle of the head and the tail,
	// where we have enough space to fit the whole trimmed value.
	pos += cwk_path_output_sized(buffer, buffer_size, pos, value, value_size);

	// Now we add the tail size to the current position and terminate the output -
	// basically, ensure that there is a '\0' at the end of the buffer.
	pos += tail_size;
	cwk_path_terminate_output(buffer, buffer_size, pos);

	// And now tell the caller how long the whole path would be.
	return pos;
}

size_t cwk_path_change_basename(const char *path, const char *new_basename,
    char *buffer, size_t buffer_size) {
	struct cwk_segment segment;
	size_t pos, root_size, new_basename_size;

	// First we try to get the last segment. We may only have a root without any
	// segments, in which case we will create one.
	if (!cwk_path_get_last_segment(path, &segment)) {
		// So there is no segment in this path. First we grab the root and output
		// that. We are not going to modify the root in any way.
		cwk_path_get_root(path, &root_size);
		pos = cwk_path_output_sized(buffer, buffer_size, 0, path, root_size);

		// We have to trim the separators from the beginning of the new basename.
		// This is quite easy to do.
		while (cwk_path_is_absolute(new_basename)) {
			++new_basename;
		}

		// Now we measure the length of the new basename, this is a two step
		// process. First we find the '\0' character at the end of the string.
		new_basename_size = 0;
		while (new_basename[new_basename_size]) {
			++new_basename_size;
		}

		// And then we trim the separators at the end of the basename until we reach
		// the first valid character.
		while ((new_basename_size > 0)
		       && cwk_path_is_absolute(&new_basename[new_basename_size - 1])) {
			--new_basename_size;
		}

		// Now we will output the new basename after the root.
		pos += cwk_path_output_sized(buffer, buffer_size, pos, new_basename,
		    new_basename_size);

		// And finally terminate the output and return the total size of the path.
		cwk_path_terminate_output(buffer, buffer_size, pos);
		return pos;
	}

	// If there is a last segment we can just forward this call, which is fairly
	// easy.
	return cwk_path_change_segment(&segment, new_basename, buffer, buffer_size);
}

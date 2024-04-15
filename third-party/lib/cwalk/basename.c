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

#include "cwalk_priv.h"

void cwk_path_get_basename(const char *path, const char **basename,
    size_t *length) {
	struct cwk_segment segment;

	// We get the last segment of the path. The last segment will contain the
	// basename if there is any. If there are no segments we will set the basename
	// to NULL and the length to 0.
	if (!cwk_path_get_last_segment(path, &segment)) {
		*basename = NULL;
		if (length) {
			*length = 0;
		}
		return;
	}

	// Now we can just output the segment contents, since that's our basename.
	// There might be trailing separators after the basename, but the size does
	// not include those.
	*basename = segment.begin;
	if (length) {
		*length = segment.size;
	}
}

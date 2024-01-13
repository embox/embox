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

size_t cwk_path_join(const char *path_a, const char *path_b, char *buffer,
    size_t buffer_size) {
	const char *paths[3];

	// This is simple. We will just create an array with the two paths which we
	// wish to join.
	paths[0] = path_a;
	paths[1] = path_b;
	paths[2] = NULL;

	// And then call the join and normalize function which will do the hard work
	// for us.
	return cwk_path_join_and_normalize_multiple(paths, buffer, buffer_size);
}

size_t cwk_path_join_multiple(const char **paths, char *buffer,
    size_t buffer_size) {
	// We can just call the internal join and normalize function for this one,
	// since it will handle everything.
	return cwk_path_join_and_normalize_multiple(paths, buffer, buffer_size);
}

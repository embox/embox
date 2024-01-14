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

size_t cwk_path_get_absolute(const char *base, const char *path, char *buffer,
    size_t buffer_size) {
	const char *paths[4];
	size_t i;

	// The basename should be an absolute path if the caller is using the API
	// correctly. However, he might not and in that case we will append a fake
	// root at the beginning.
	if (cwk_path_is_absolute(base)) {
		i = 0;
	}
	else {
		paths[0] = "/";
		i = 1;
	}

	if (cwk_path_is_absolute(path)) {
		// If the submitted path is not relative the base path becomes irrelevant.
		// We will only normalize the submitted path instead.
		paths[i++] = path;
		paths[i] = NULL;
	}
	else {
		// Otherwise we append the relative path to the base path and normalize it.
		// The result will be a new absolute path.
		paths[i++] = base;
		paths[i++] = path;
		paths[i] = NULL;
	}

	// Finally join everything together and normalize it.
	return cwk_path_join_and_normalize_multiple(paths, buffer, buffer_size);
}

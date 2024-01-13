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

void cwk_path_get_root(const char *path, size_t *length) {
	// The slash of the unix path represents the root. There is no root if there
	// is no slash.
	*length = (size_t)cwk_path_is_absolute(path);
}

size_t cwk_path_change_root(const char *path, const char *new_root,
    char *buffer, size_t buffer_size) {
	const char *tail;
	size_t root_length, path_length, tail_length, new_root_length,
	    new_path_size;

	// First we need to determine the actual size of the root which we will
	// change.
	cwk_path_get_root(path, &root_length);

	// Now we determine the sizes of the new root and the path. We need that to
	// determine the size of the part after the root (the tail).
	new_root_length = strlen(new_root);
	path_length = strlen(path);

	// Okay, now we calculate the position of the tail and the length of it.
	tail = path + root_length;
	tail_length = path_length - root_length;

	// We first output the tail and then the new root, that's because the source
	// path and the buffer may be overlapping. This way the root will not
	// overwrite the tail.
	cwk_path_output_sized(buffer, buffer_size, new_root_length, tail,
	    tail_length);
	cwk_path_output_sized(buffer, buffer_size, 0, new_root, new_root_length);

	// Finally we calculate the size o the new path and terminate the output with
	// a '\0'.
	new_path_size = tail_length + new_root_length;
	cwk_path_terminate_output(buffer, buffer_size, new_path_size);

	return new_path_size;
}
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

#ifndef THIRD_PARTY_LIB_CWALK_PRIV_H_
#define THIRD_PARTY_LIB_CWALK_PRIV_H_

#include <stdbool.h>
#include <stddef.h>

#include <lib/cwalk.h>

/**
 * A joined path represents multiple path strings which are concatenated, but
 * not (necessarily) stored in contiguous memory. The joined path allows to
 * iterate over the segments as if it was one piece of path.
 */
struct cwk_segment_joined {
	struct cwk_segment segment;
	const char **paths;
	size_t path_index;
};

extern size_t cwk_path_output_sized(char *buffer, size_t buffer_size,
    size_t position, const char *str, size_t length);

extern size_t cwk_path_output_current(char *buffer, size_t buffer_size,
    size_t position);

extern size_t cwk_path_output_back(char *buffer, size_t buffer_size,
    size_t position);

extern size_t cwk_path_output_separator(char *buffer, size_t buffer_size,
    size_t position);

extern void cwk_path_terminate_output(char *buffer, size_t buffer_size,
    size_t pos);

extern const char *cwk_path_find_next_stop(const char *c);

extern bool cwk_path_get_first_segment_without_root(const char *path,
    const char *segments, struct cwk_segment *segment);

extern bool cwk_path_get_last_segment_without_root(const char *path,
    struct cwk_segment *segment);

extern bool cwk_path_get_first_segment_joined(const char **paths,
    struct cwk_segment_joined *sj);

extern bool cwk_path_get_next_segment_joined(struct cwk_segment_joined *sj);

extern bool cwk_path_get_previous_segment_joined(struct cwk_segment_joined *sj);

extern bool cwk_path_segment_back_will_be_removed(
    struct cwk_segment_joined *sj);

extern bool cwk_path_segment_normal_will_be_removed(
    struct cwk_segment_joined *sj);

extern bool cwk_path_segment_will_be_removed(
    const struct cwk_segment_joined *sj, bool absolute);

extern bool cwk_path_segment_joined_skip_invisible(
    struct cwk_segment_joined *sj, bool absolute);

extern bool cwk_path_is_string_equal(const char *first, const char *second,
    size_t first_size, size_t second_size);

extern size_t cwk_path_join_and_normalize_multiple(const char **paths,
    char *buffer, size_t buffer_size);

#endif /* THIRD_PARTY_LIB_CWALK_PRIV_H_ */

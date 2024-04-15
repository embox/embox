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

size_t cwk_path_output_sized(char *buffer, size_t buffer_size, size_t position,
    const char *str, size_t length) {
	size_t amount_written;

	// First we determine the amount which we can write to the buffer. There are
	// three cases. In the first case we have enough to store the whole string in
	// it. In the second one we can only store a part of it, and in the third we
	// have no space left.
	if (buffer_size > position + length) {
		amount_written = length;
	}
	else if (buffer_size > position) {
		amount_written = buffer_size - position;
	}
	else {
		amount_written = 0;
	}

	// If we actually want to write out something we will do that here. We will
	// always append a '\0', this way we are guaranteed to have a valid string at
	// all times.
	if (amount_written > 0) {
		memmove(&buffer[position], str, amount_written);
	}

	// Return the theoretical length which would have been written when everything
	// would have fit in the buffer.
	return length;
}

size_t cwk_path_output_current(char *buffer, size_t buffer_size,
    size_t position) {
	// We output a "current" directory, which is a single character. This
	// character is currently not style dependant.
	return cwk_path_output_sized(buffer, buffer_size, position, ".", 1);
}

size_t cwk_path_output_back(char *buffer, size_t buffer_size, size_t position) {
	// We output a "back" directory, which ahs two characters. This
	// character is currently not style dependant.
	return cwk_path_output_sized(buffer, buffer_size, position, "..", 2);
}

size_t cwk_path_output_separator(char *buffer, size_t buffer_size,
    size_t position) {
	// We output a separator, which is a single character.
	return cwk_path_output_sized(buffer, buffer_size, position, "/", 1);
}

void cwk_path_terminate_output(char *buffer, size_t buffer_size, size_t pos) {
	if (buffer_size > 0) {
		if (pos >= buffer_size) {
			buffer[buffer_size - 1] = '\0';
		}
		else {
			buffer[pos] = '\0';
		}
	}
}

const char *cwk_path_find_next_stop(const char *c) {
	// We just move forward until we find a '\0' or a separator, which will be our
	// next "stop".
	while (*c != '\0' && cwk_path_is_relative(c)) {
		++c;
	}

	// Return the pointer of the next stop.
	return c;
}

bool cwk_path_get_first_segment_without_root(const char *path,
    const char *segments, struct cwk_segment *segment) {
	// Let's remember the path. We will move the path pointer afterwards, that's
	// why this has to be done first.
	segment->path = path;
	segment->segments = segments;
	segment->begin = segments;
	segment->end = segments;
	segment->size = 0;

	// Now let's check whether this is an empty string. An empty string has no
	// segment it could use.
	if (*segments == '\0') {
		return false;
	}

	// If the string starts with separators, we will jump over those. If there is
	// only a slash and a '\0' after it, we can't determine the first segment
	// since there is none.
	while (cwk_path_is_absolute(segments)) {
		++segments;
		if (*segments == '\0') {
			return false;
		}
	}

	// So this is the beginning of our segment.
	segment->begin = segments;

	// Now let's determine the end of the segment, which we do by moving the path
	// pointer further until we find a separator.
	segments = cwk_path_find_next_stop(segments);

	// And finally, calculate the size of the segment by subtracting the position
	// from the end.
	segment->size = (size_t)(segments - segment->begin);
	segment->end = segments;

	// Tell the caller that we found a segment.
	return true;
}

bool cwk_path_get_last_segment_without_root(const char *path,
    struct cwk_segment *segment) {
	// Now this is fairly similar to the normal algorithm, however, it will assume
	// that there is no root in the path. So we grab the first segment at this
	// position, assuming there is no root.
	if (!cwk_path_get_first_segment_without_root(path, path, segment)) {
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

bool cwk_path_get_first_segment_joined(const char **paths,
    struct cwk_segment_joined *sj) {
	// Prepare the first segment. We position the joined segment on the first path
	// and assign the path array to the struct.
	sj->path_index = 0;
	sj->paths = paths;

	// We loop through all paths until we find one which has a segment. The result
	// is stored in a variable, so we can let the caller know whether we found one
	// or not.
	while (paths[sj->path_index] != NULL) {
		if (cwk_path_get_first_segment(paths[sj->path_index], &sj->segment)) {
			return true;
		}
		++sj->path_index;
	}

	return false;
}

bool cwk_path_get_next_segment_joined(struct cwk_segment_joined *sj) {
	bool result;

	if (sj->paths[sj->path_index] == NULL) {
		// We reached already the end of all paths, so there is no other segment
		// left.
		return false;
	}
	else if (cwk_path_get_next_segment(&sj->segment)) {
		// There was another segment on the current path, so we are good to
		// continue.
		return true;
	}

	// We try to move to the next path which has a segment available. We must at
	// least move one further since the current path reached the end.
	result = false;

	do {
		++sj->path_index;

		// And we obviously have to stop this loop if there are no more paths left.
		if (sj->paths[sj->path_index] == NULL) {
			break;
		}

		// Grab the first segment of the next path and determine whether this path
		// has anything useful in it. There is one more thing we have to consider
		// here - for the first time we do this we want to skip the root, but
		// afterwards we will consider that to be part of the segments.
		result = cwk_path_get_first_segment_without_root(
		    sj->paths[sj->path_index], sj->paths[sj->path_index], &sj->segment);

	} while (!result);

	// Finally, report the result back to the caller.
	return result;
}

bool cwk_path_get_previous_segment_joined(struct cwk_segment_joined *sj) {
	bool result;

	if (*sj->paths == NULL) {
		// It's possible that there is no initialized segment available in the
		// struct since there are no paths. In that case we can return false, since
		// there is no previous segment.
		return false;
	}
	else if (cwk_path_get_previous_segment(&sj->segment)) {
		// Now we try to get the previous segment from the current path. If we can
		// do that successfully, we can let the caller know that we found one.
		return true;
	}

	result = false;

	do {
		// We are done once we reached index 0. In that case there are no more
		// segments left.
		if (sj->path_index == 0) {
			break;
		}

		// There is another path which we have to inspect. So we decrease the path
		// index.
		--sj->path_index;

		// If this is the first path we will have to consider that this path might
		// include a root, otherwise we just treat is as a segment.
		if (sj->path_index == 0) {
			result = cwk_path_get_last_segment(sj->paths[sj->path_index],
			    &sj->segment);
		}
		else {
			result = cwk_path_get_last_segment_without_root(
			    sj->paths[sj->path_index], &sj->segment);
		}

	} while (!result);

	return result;
}

bool cwk_path_segment_back_will_be_removed(struct cwk_segment_joined *sj) {
	enum cwk_segment_type type;
	int counter;

	// We are handling back segments here. We must verify how many back segments
	// and how many normal segments come before this one to decide whether we keep
	// or remove it.

	// The counter determines how many normal segments are our current segment,
	// which will popped off before us. If the counter goes above zero it means
	// that our segment will be popped as well.
	counter = 0;

	// We loop over all previous segments until we either reach the beginning,
	// which means our segment will not be dropped or the counter goes above zero.
	while (cwk_path_get_previous_segment_joined(sj)) {
		// Now grab the type. The type determines whether we will increase or
		// decrease the counter. We don't handle a CWK_CURRENT frame here since it
		// has no influence.
		type = cwk_path_get_segment_type(&sj->segment);
		if (type == CWK_NORMAL) {
			// This is a normal segment. The normal segment will increase the counter
			// since it neutralizes one back segment. If we go above zero we can
			// return immediately.
			++counter;
			if (counter > 0) {
				return true;
			}
		}
		else if (type == CWK_BACK) {
			// A CWK_BACK segment will reduce the counter by one. We can not remove a
			// back segment as long we are not above zero since we don't have the
			// opposite normal segment which we would remove.
			--counter;
		}
	}

	// We never got a count larger than zero, so we will keep this segment alive.
	return false;
}

bool cwk_path_segment_normal_will_be_removed(struct cwk_segment_joined *sj) {
	enum cwk_segment_type type;
	int counter;

	// The counter determines how many segments are above our current segment,
	// which will popped off before us. If the counter goes below zero it means
	// that our segment will be popped as well.
	counter = 0;

	// We loop over all following segments until we either reach the end, which
	// means our segment will not be dropped or the counter goes below zero.
	while (cwk_path_get_next_segment_joined(sj)) {
		// First, grab the type. The type determines whether we will increase or
		// decrease the counter. We don't handle a CWK_CURRENT frame here since it
		// has no influence.
		type = cwk_path_get_segment_type(&sj->segment);
		if (type == CWK_NORMAL) {
			// This is a normal segment. The normal segment will increase the counter
			// since it will be removed by a "../" before us.
			++counter;
		}
		else if (type == CWK_BACK) {
			// A CWK_BACK segment will reduce the counter by one. If we are below zero
			// we can return immediately.
			--counter;
			if (counter < 0) {
				return true;
			}
		}
	}

	// We never got a negative count, so we will keep this segment alive.
	return false;
}

bool cwk_path_segment_will_be_removed(const struct cwk_segment_joined *sj,
    bool absolute) {
	enum cwk_segment_type type;
	struct cwk_segment_joined sjc;

	// We copy the joined path so we don't need to modify it.
	sjc = *sj;

	// First we check whether this is a CWK_CURRENT or CWK_BACK segment, since
	// those will always be dropped.
	type = cwk_path_get_segment_type(&sj->segment);
	if (type == CWK_CURRENT || (type == CWK_BACK && absolute)) {
		return true;
	}
	else if (type == CWK_BACK) {
		return cwk_path_segment_back_will_be_removed(&sjc);
	}
	else {
		return cwk_path_segment_normal_will_be_removed(&sjc);
	}
}

bool cwk_path_segment_joined_skip_invisible(struct cwk_segment_joined *sj,
    bool absolute) {
	while (cwk_path_segment_will_be_removed(sj, absolute)) {
		if (!cwk_path_get_next_segment_joined(sj)) {
			return false;
		}
	}

	return true;
}

bool cwk_path_is_string_equal(const char *first, const char *second,
    size_t first_size, size_t second_size) {
	// The two strings are not equal if the sizes are not equal.
	if (first_size != second_size) {
		return false;
	}

	// If the path style is UNIX, we will compare case sensitively. This can be
	// done easily using strncmp.
	return strncmp(first, second, first_size) == 0;
}

size_t cwk_path_join_and_normalize_multiple(const char **paths, char *buffer,
    size_t buffer_size) {
	size_t pos;
	bool absolute, has_segment_output;
	struct cwk_segment_joined sj;

	// We initialize the position after the root, which should get us started.
	cwk_path_get_root(paths[0], &pos);

	// Determine whether the path is absolute or not. We need that to determine
	// later on whether we can remove superfluous "../" or not.
	absolute = cwk_path_is_absolute(paths[0]);

	// First copy the root to the output. After copying, we will normalize the
	// root.
	cwk_path_output_sized(buffer, buffer_size, 0, paths[0], pos);

	// So we just grab the first segment. If there is no segment we will always
	// output a "/", since we currently only support absolute paths here.
	if (!cwk_path_get_first_segment_joined(paths, &sj)) {
		goto done;
	}

	// Let's assume that we don't have any segment output for now. We will toggle
	// this flag once there is some output.
	has_segment_output = false;

	do {
		// Check whether we have to drop this segment because of resolving a
		// relative path or because it is a CWK_CURRENT segment.
		if (cwk_path_segment_will_be_removed(&sj, absolute)) {
			continue;
		}

		// We add a separator if we previously wrote a segment. The last segment
		// must not have a trailing separator. This must happen before the segment
		// output, since we would override the null terminating character with
		// reused buffers if this was done afterwards.
		if (has_segment_output) {
			pos += cwk_path_output_separator(buffer, buffer_size, pos);
		}

		// Remember that we have segment output, so we can handle the trailing slash
		// later on. This is necessary since we might have segments but they are all
		// removed.
		has_segment_output = true;

		// Write out the segment but keep in mind that we need to follow the
		// buffer size limitations. That's why we use the path output functions
		// here.
		pos += cwk_path_output_sized(buffer, buffer_size, pos, sj.segment.begin,
		    sj.segment.size);
	} while (cwk_path_get_next_segment_joined(&sj));

	// Remove the trailing slash, but only if we have segment output. We don't
	// want to remove anything from the root.
	if (!has_segment_output && pos == 0) {
		// This may happen if the path is absolute and all segments have been
		// removed. We can not have an empty output - and empty output means we stay
		// in the current directory. So we will output a ".".
		assert(absolute == false);
		pos += cwk_path_output_current(buffer, buffer_size, pos);
	}

	// We must append a '\0' in any case, unless the buffer size is zero. If the
	// buffer size is zero, which means we can not.
done:
	cwk_path_terminate_output(buffer, buffer_size, pos);

	// And finally let our caller know about the total size of the normalized
	// path.
	return pos;
}

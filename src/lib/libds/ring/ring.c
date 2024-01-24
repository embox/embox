/**
 * @file
 *
 * @date 20.03.13
 * @author Eldar Abusalimov
 */

#include <lib/libds/ring.h>

#include <string.h>

size_t ring_read(struct ring *r, size_t r_size, size_t read_size) {
	return ring_just_read(r, r_size, ring_can_read(r, r_size, read_size));
}

size_t ring_write(struct ring *r, size_t r_size, size_t write_size) {
	return ring_just_write(r, r_size, ring_can_write(r, r_size, write_size));
}

size_t ring_read_all_into(struct ring *r, const char *r_buff, size_t r_size,
		char *into_buff, size_t read_size) {
	size_t count = 0;
	size_t block_size;

	while ((block_size = ring_can_read(r, r_size, read_size))) {
		memcpy(into_buff + count, r_buff + r->tail, block_size);
		read_size -= block_size;
		count += block_size;
		ring_just_read(r, r_size, block_size);
	}

	return count;
}

size_t ring_write_all_from(struct ring *r, char *r_buff, size_t r_size,
		const char *from_buff, size_t write_size) {
	size_t count = 0;
	size_t block_size;

	while ((block_size = ring_can_write(r, r_size, write_size))) {
		memcpy(r_buff + r->head, from_buff + count, block_size);
		write_size -= block_size;
		count += block_size;
		ring_just_write(r, r_size, block_size);
	}

	return count;
}

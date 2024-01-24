/**
 * @file
 * @brief Building block for constructing ring buffers.
 *
 * @date 19.03.13
 * @author Eldar Abusalimov
 */

#ifndef UTIL_RING_H_
#define UTIL_RING_H_

#include <assert.h>
#include <stddef.h>

#include <util/math.h>

/**
 * Control block for managing ring buffers.
 *
 * The implementation uses two pointers (offets info a real buffer).
@verbatim
        [------**************************-----]
          tail-^                         ^-head
@endverbatim
 *
 * One element is always reserved to distinguish between empty and full state,
 * see #ring_empty() and #ring_full().
 *
 * Performing real operations on data (typically, a #memcpy()) is up to the
 * client code, which manages memory and provides proper locking policy.
 */
struct ring {
	size_t head; /* where to write */
	size_t tail; /* from where to read */
};

/** Initializes an empty buffer. @return the argument. */
static inline struct ring *ring_init(struct ring *r) {
	r->head = r->tail = 0;
	return r;
}

static inline void __ring_assert_invariants(struct ring *r, size_t r_size) {
	assertf(r->head < r_size, "Did you forget to call %s?", "ring_fixup_head");
	assertf(r->tail < r_size, "Did you forget to call %s?", "ring_fixup_tail");
}

/**
 * Buffer is considered empty when two pointers are the same: tail == head
@verbatim
        [-------------------------------------]
          tail-^-head
@endverbatim
 */
static inline int ring_empty(struct ring *r) {
	return (r->head == r->tail);
}

/**
 * Buffer is full when tail == (head-1), maybe wrapped.
@verbatim
        [************************************-]
         ^-tail                         head-^
        [*************-***********************]
                 head-^^-tail
@endverbatim
 */
static inline int ring_full(struct ring *r, size_t r_size) {
	__ring_assert_invariants(r, r_size);
	return r->head == (r->tail ? r->tail : r_size) - 1;
}

/**
 * Buffer data may wrap, i.e. there may be that (tail > head):
@verbatim
        [*************--------------**********]
                 head-^             ^-tail
@endverbatim
 */
static inline int ring_wraps(struct ring *r, size_t r_size) {
	__ring_assert_invariants(r, r_size);
	return (r->head < r->tail);
}

/*
 * ring_xxx_size get total amount of data available for reading or writing.
 */

static inline size_t ring_data_size(struct ring *r, size_t r_size) {
	size_t data_size = (r->head - r->tail);
	if (ring_wraps(r, r_size))
		data_size += r_size;
	return data_size;
}

static inline size_t ring_room_size(struct ring *r, size_t r_size) {
	size_t room_size = (r->tail - r->head - 1);
	if (!ring_wraps(r, r_size))
		room_size += r_size;
	return room_size;
}

/*
 * ring_can_xxx tell how many _contiguous_ bytes can be read or written.
 */

/** @return Amount of requested bytes that can be read without wrapping. */
static inline size_t ring_can_read(struct ring *r, size_t r_size,
		size_t read_size) {
	size_t nwrap_data_end = ring_wraps(r, r_size)
			? r_size
			: r->head;
	return min(read_size, nwrap_data_end - r->tail);
}

/** @return Amount of requested bytes that can be written without wrapping. */
static inline size_t ring_can_write(struct ring *r, size_t r_size,
		size_t write_size) {
	size_t nwrap_room_end = !ring_wraps(r, r_size)
			? r_size - (!r->tail)
			: r->tail - 1;
	return min(write_size, nwrap_room_end - r->head);
}

/* Mutator methods. */

/*
 * ring_fixup_xxx fix head or tail in case of wrapping.
 */

/** @return New value of tail. */
static inline size_t ring_fixup_tail(struct ring *r, size_t r_size) {
	if (r->tail >= r_size)
		r->tail -= r_size;
	return r->tail;
}

/** @return New value of head. */
static inline size_t ring_fixup_head(struct ring *r, size_t r_size) {
	if (r->head >= r_size)
		r->head -= r_size;
	return r->head;
}

/*
 * ring_just_xxx must be called after real read/write operation.
 * Size of operation must not exceed a value returned by ring_can_xxx.
 */

/** @return The last argument (requested size). */
static inline size_t ring_just_read(struct ring *r, size_t r_size,
		size_t read_size) {
	assert(read_size <= ring_can_read(r, r_size, read_size));
	r->tail += read_size;
	ring_fixup_tail(r, r_size);
	return read_size;
}

/** @return The last argument (requested size). */
static inline size_t ring_just_write(struct ring *r, size_t r_size,
		size_t write_size) {
	assert(write_size <= ring_can_write(r, r_size, write_size));
	r->head += write_size;
	ring_fixup_head(r, r_size);
	return write_size;
}

/*
 * ring_xxx simply adjust the pointers effectively emulating read or write
 * of contiguous bytes.
 */

/** @return How many bytes were read without wrapping. */
extern size_t ring_read(struct ring *r, size_t r_size, size_t read_size);

/** @return How many bytes were written without wrapping. */
extern size_t ring_write(struct ring *r, size_t r_size, size_t write_size);

/*
 * The following two functions read/write into/from user-provided buffer,
 * handling all available data/room, even wrapped.
 */

/** @return How many bytes were read */
extern size_t ring_read_all_into(struct ring *r,
		const char *r_buff, size_t r_size,
		char *into_buff, size_t read_size);

/** @return How many bytes were written */
extern size_t ring_write_all_from(struct ring *r,
		char *r_buff, size_t r_size,
		const char *from_buff, size_t write_size);

#endif /* UTIL_RING_H_ */

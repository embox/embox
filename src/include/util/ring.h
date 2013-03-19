/**
 * @file
 * @brief Building block for constructin ring buffers.
 *
 * @date 19.03.13
 * @author Eldar Abusalimov
 */

#ifndef UTIL_RING_H_
#define UTIL_RING_H_

#include <assert.h>
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
 * Performing real opeations on data (typically, a #memcpy()) is up to the
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

/**
 * Buffer is considered empty when two pointers are the same: tail == head
@verbatim
        [-------------------------------------]
          tail-^-head
@endverbatim
 */
static inline int ring_empty(struct ring *r) {
	return r->head == r->tail;
}

/**
 * Buffer is full when tail == (head-1), maybe wrapped.
@verbatim
        [*************-***********************]
                 head-^^-tail
@endverbatim
 */
static inline int ring_full(struct ring *r, size_t r_size) {
	return r->head == (r->tail ? r->tail - 1 : r_size);
}

/**
 * Buffer data may wrap, i.e. there may be that (tail > head):
@verbatim
        [*************--------------**********]
                 head-^             ^-tail
@endverbatim
 */
static inline int ring_wraps(struct ring *r, size_t r_size) {
	assert(r->head <= r_size, "Did you forget to call %s?", "ring_fixup_head");
	assert(r->tail <  r_size, "Did you forget to call %s?", "ring_fixup_tail");
	return (r->head < r->tail);
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
	/* unlike ring_fixup_tail, a strict comparison is used here */
	if (r->head > r_size)
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
 * ring_xxx simply read or write contiguous bytes, adjusting the pointers.
 */

/** @return How many bytes were read without wrapping. */
static inline size_t ring_read(struct ring *r, size_t r_size,
		size_t read_size) {
	return ring_just_read(r, r_size, ring_can_read(r, r_size, read_size));
}

/** @return How many bytes were written without wrapping. */
static inline size_t ring_write(struct ring *r, size_t r_size,
		size_t write_size) {
	return ring_just_write(r, r_size, ring_can_write(r, r_size, write_size));
}

#endif /* UTIL_RING_H_ */

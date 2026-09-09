/**
 * @file
 * @brief TODO documentation for emit.c -- Eldar Abusalimov
 *
 * @date Apr 2, 2012
 * @author Eldar Abusalimov
 */

#include <framework/test/emit.h>
#include <hal/ipl.h>
#include <kernel/spinlock.h>

/* One lock for every buffer: masking interrupts does not exclude other
 * cores, and every thread a suite starts writes here. */
static spinlock_t emit_lock = SPIN_STATIC_UNLOCKED;

void test_emit_into(struct test_emit_buffer *b, char ch) {
	ipl_t ipl = spin_lock_ipl(&emit_lock);
	{
		if (test_emit_buffer_overflown(b)) {
			goto out;
		}

		if (!test_emit_buffer_full(b)) {
			*b->ptr++ = ch;
			*b->ptr = '\0';
		}
		else {
			/* do overflow */
			b->ptr++;
		}
	}
out:
	spin_unlock_ipl(&emit_lock, ipl);
}

/**
 * @file
 * @brief TODO documentation for emit.c -- Eldar Abusalimov
 *
 * @date Apr 2, 2012
 * @author Eldar Abusalimov
 */

#include <framework/test/emit.h>
#include <hal/ipl.h>

void test_emit_into(struct test_emit_buffer *b, char ch) {
	ipl_t ipl = ipl_save();
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
	ipl_restore(ipl);
}

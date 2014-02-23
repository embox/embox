/**
 * @file
 * @brief Implementation of Big Kernel Lock
 *
 * @date 08.02.12
 * @author Anton Bulychev
 * @author Ilia Vaprol
 */

#include <assert.h>

#include <hal/ipl.h>
#include <hal/cpu.h>
#include <kernel/spinlock.h>

static spinlock_t bkl = SPIN_STATIC_UNLOCKED;

void bkl_lock(void) {
	__spin_lock(&bkl);
}

void bkl_unlock(void) {
	__spin_unlock(&bkl);
}

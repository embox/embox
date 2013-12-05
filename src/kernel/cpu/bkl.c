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

static spinlock_t bkl = SPIN_UNLOCKED;

static unsigned int owner_id = -1;
static unsigned int nested = 0;

void bkl_lock(void) {
	ipl_t ipl;
	int locked;
	unsigned int cpu_id;

	locked = 0;
	cpu_id = cpu_get_id();

	do {
		ipl = spin_lock_ipl(&bkl);
		{
			if ((nested == 0) || (cpu_id == owner_id)) {
				owner_id = cpu_id;
				++nested;

				locked = 1;
			}
		}
		spin_unlock_ipl(&bkl, ipl);
	} while (!locked);
}

void bkl_unlock(void) {
	ipl_t ipl;

	ipl = spin_lock_ipl(&bkl);
	{
		assert(nested && (cpu_get_id() == owner_id));
		--nested;
		if (!nested)
			owner_id = -1;
	}
	spin_unlock_ipl(&bkl, ipl);
}

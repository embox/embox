/**
 * @file
 * @brief Implementation of Big Kernel Lock
 *
 * @date 08.02.12
 * @author Anton Bulychev
 * @author Ilia Vaprol
 */

#include <hal/ipl.h>
#include <hal/cpu.h>
#include <kernel/spinlock.h>

static spinlock_t bkl = SPIN_UNLOCKED;

static unsigned int owner_id, nested = 0;

void bkl_lock(void) {
	ipl_t ipl;
	int locked;
	unsigned int cpu_id;

	locked = 0;
	cpu_id = cpu_get_id();

	do {
		ipl = ipl_save();
		spin_lock(&bkl);
		{
			if ((nested == 0) || (cpu_id == owner_id)) {
				owner_id = cpu_id;
				++nested;

				locked = 1;
			}
		}
		spin_unlock(&bkl);
		ipl_restore(ipl);
	} while (!locked);
}

void bkl_unlock(void) {
	ipl_t ipl;

	ipl = ipl_save();
	spin_lock(&bkl);
	{
		--nested;
	}
	spin_unlock(&bkl);
	ipl_restore(ipl);
}

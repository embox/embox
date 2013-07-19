/**
 * @file
 * @brief Implementation of Big Kernel Lock
 *
 * @date 08.02.2012
 * @author Anton Bulychev
 */

#include <hal/ipl.h>
#include <kernel/cpu.h>
#include <kernel/spinlock.h>

static spinlock_t bkl = SPIN_UNLOCKED;

static unsigned int owner_id, nested = 0;

void bkl_lock(void) {
	ipl_t ipl;
	int cpu_id, tmp = 0;

	while (1) {
		ipl = ipl_save();

		spin_lock(&bkl);

		cpu_id = cpu_get_id();

		if (!nested || cpu_id == owner_id) {
			owner_id = cpu_id;
			nested++;

			tmp = 1;
		}

		spin_unlock(&bkl);

		ipl_restore(ipl);

		if (tmp) break;
	}
}

void bkl_unlock(void) {
	ipl_t ipl = ipl_save();

	spin_lock(&bkl);

	nested--;

	spin_unlock(&bkl);

	ipl_restore(ipl);
}

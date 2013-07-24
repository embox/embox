/**
 * @file
 * @brief
 *
 * @date 24.07.13
 * @author Ilia Vaprol
 */

#include <embox/test.h>
#include <kernel/spinlock.h>

EMBOX_TEST_SUITE("kernel/spinlock tests");

TEST_CASE("spin_lock can lock after initializing") {
	spinlock_t l = SPIN_UNLOCKED;

	test_assert_not_zero(spin_trylock(&l));
	spin_unlock(&l);
}

TEST_CASE("spinlock is reusable object") {
	spinlock_t l = SPIN_UNLOCKED;

	test_assert_not_zero(spin_trylock(&l));
	spin_unlock(&l);

	test_assert_not_zero(spin_trylock(&l));
	spin_unlock(&l);
}

TEST_CASE("spin_lock cann't lock one object twice") {
	spinlock_t l = SPIN_UNLOCKED;

	test_assert_not_zero(spin_trylock(&l));

	test_assert_zero(spin_trylock(&l));

	spin_unlock(&l);
}

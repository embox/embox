/**
 * @file
 * @brief MMU test suite for complex testing mmu core support
 *
 * @date 28.07.11
 * @author Anton Kozlov
 */

#include <embox/test.h>
#include <framework/test/assert.h>

#include <hal/mm/mmu_core.h>
#include <hal/mm/mmu_page.h>

#include <hal/env/traps_core.h>

#include <hal/test/mmu_core.h>
#include <hal/test/traps_core.h>

EMBOX_TEST_SUITE("Complex MMU core support test suite");

TEST_SETUP(mmu_case_setup);

TEST_TEARDOWN(mmu_case_teardown);

/*mapping to address, assuming there is no mem at here */
#define BIGADDR 0xf0000000
/* Unique values that seems to pretty to accidentally happen in mem,
 * used to read-n-check */
#define UNIQ_VAL_1 0x12345678
#define UNIQ_VAL_2 0x87654321

/* Minimal area size to mapping */
#define PAGE_SIZE 0x1000

/* Big area able to cover text + data + stack*/
#define REGION_SIZE 0x1000000

#define VADDR(region, phyaddr, map_size) (region + ((uint32_t)(phyaddr) & (map_size - 1)))

TEST_CASE("MMU enabling should go alright. "
		"Program flow must not halts.") {
	mmu_on();
	test_assert_zero(0);
}

TEST_CASE("Read/write operations must execute in MMU-mode.") {
	uint32_t addr = 0;
	uint32_t vaddr = VADDR(BIGADDR, &addr, PAGE_SIZE);

	mmu_map_region((mmu_ctx_t)0, (paddr_t) &addr & ~(PAGE_SIZE - 1),
			BIGADDR, PAGE_SIZE,
			MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE);

	mmu_on();

	test_assert_equal((*((volatile uint32_t *) vaddr)), addr);
			//"Mapping and mapped value is not equals");

	/* test read/write */
	*((volatile uint32_t *) vaddr) = UNIQ_VAL_1;
	test_assert_equal(*((volatile uint32_t *) vaddr), UNIQ_VAL_1);
}

static int simple_dfault_happend = 0;

/* MMU data access exception handler */
static int simple_dfault_handler(uint32_t trap_nr, void *data) {
	simple_dfault_happend = 1;
	return 0;
}

TEST_CASE("Writing in read-only memory should generate exception."
		  " Vaule must left untouched.") {
	uint32_t addr = UNIQ_VAL_1;
	uint32_t vaddr = VADDR(BIGADDR, &addr, PAGE_SIZE);

	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DATA_SECUR_FAULT, simple_dfault_handler);

	mmu_map_region((mmu_ctx_t)0, (paddr_t)(&addr) & ~0xfff, BIGADDR, PAGE_SIZE,
				MMU_PAGE_CACHEABLE | MMU_PAGE_EXECUTEABLE);
	mmu_on();

	/* Data access exception */
	*((volatile uint32_t *) vaddr) = UNIQ_VAL_2;

	test_assert(addr == UNIQ_VAL_1);
	test_assert(simple_dfault_happend);
}

/* MMU data access exception handler */
static int dfault_handler(uint32_t trap_nr, void *data) {
	mmu_page_set_flags((mmu_ctx_t) 0x0, BIGADDR, MMU_PAGE_WRITEABLE);
	/* repeat instruction */
	return 1;
}

TEST_CASE("Writing to read-only memory should cause exception."
		 " Exception handler should allow write.") {
	uint32_t addr = UNIQ_VAL_1;
	uint32_t vaddr = VADDR(BIGADDR, &addr, PAGE_SIZE);
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DATA_SECUR_FAULT, dfault_handler);

	mmu_map_region((mmu_ctx_t)0, (paddr_t)&addr & ~(PAGE_SIZE - 1), BIGADDR, PAGE_SIZE,
			MMU_PAGE_CACHEABLE);

	mmu_on();

	*((volatile uint32_t *) vaddr) = UNIQ_VAL_1;
	test_assert_equal(addr, UNIQ_VAL_1);
}

uint8_t __attribute__ ((aligned (PAGE_SIZE))) page[PAGE_SIZE];
static int flag = 0;

static int pagefault_handler(uint32_t nr, void *data) {
	int err_addr = mmu_get_fault_address() & ~(PAGE_SIZE - 1);
	mmu_off();

	mmu_map_region((mmu_ctx_t) 0, (paddr_t) ((unsigned long) &page),
			(vaddr_t) err_addr, PAGE_SIZE, MMU_PAGE_CACHEABLE |
			MMU_PAGE_WRITEABLE);

	flag = 1;
	mmu_on();
	return 1; // execute exception-cause instruction once more
}

TEST_CASE("Pagefault should be considered right.") {
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DATA_MISS_FAULT,
				&pagefault_handler); //data mmu miss

	mmu_on();

	*((uint32_t *) BIGADDR) = UNIQ_VAL_1;

	test_assert_equal(*((unsigned long *) BIGADDR), UNIQ_VAL_1);
}

static mmu_env_t prev_mmu_env;
static traps_env_t old_env;

static int mmu_case_setup(void) {
	extern char _text_start;
	mmu_env_t prev_mmu_env;
	traps_env_t old_env;

	mmu_save_env(&prev_mmu_env);
	mmu_set_env(testmmu_env());

	traps_save_env(&old_env);
	traps_set_env(testtraps_env());
	mmu_map_region((mmu_ctx_t)0, (uint32_t) &_text_start,
		(uint32_t) &_text_start, REGION_SIZE,
		MMU_PAGE_CACHEABLE | MMU_PAGE_WRITEABLE | MMU_PAGE_EXECUTEABLE);

	return 0;
}

static int mmu_case_teardown(void) {
	traps_restore_env(&old_env);
	mmu_restore_env(&prev_mmu_env);
	return 0;
}

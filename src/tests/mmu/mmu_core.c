/**
 * @file
 * @brief MMU test suite for complex testing mmu core support
 *
 * @date 28.07.11
 * @author Anton Kozlov
 */

#include <embox/test.h>
#include <framework/test/assert.h>

#include <mem/vmem.h>
#include <hal/test/traps_core.h>

EMBOX_TEST_SUITE("Complex MMU core support test suite");

TEST_SETUP(mmu_case_setup);

TEST_TEARDOWN(mmu_case_teardown);

/*mapping to address, assuming there is no mem at here */
#define BIGADDR1 0xf0010000
#define BIGADDR2 0xf0011000

/* Unique values that seems to pretty to accidentally happen in mem,
 * used to read-n-check */
#define UNIQ_VAL_1 0x12345678
#define UNIQ_VAL_2 0x87654321

/* Minimal area size to mapping */
#define PAGE_SIZE 0x1000

/* Big area able to cover text + data + stack*/
#define REGION_SIZE 0x1000000

#define VADDR(region, phyaddr, map_size) (region + ((uint32_t)(phyaddr) & (map_size - 1)))

#if 0
TEST_CASE("Read/write operations must execute in MMU-mode.") {
	uint32_t addr = 0;
	uint32_t vaddr = VADDR(BIGADDR, &addr, PAGE_SIZE);

	vmem_map_region((mmu_ctx_t)0, (mmu_paddr_t) &addr & ~(PAGE_SIZE - 1),
			BIGADDR, PAGE_SIZE, VMEM_PAGE_CACHEABLE | VMEM_PAGE_WRITABLE);

	test_assert_equal((*((volatile uint32_t *) vaddr)), addr);
			//"Mapping and mapped value is not equals");

	/* test read/write */
	*((volatile uint32_t *) vaddr) = UNIQ_VAL_1;
	test_assert_equal(*((volatile uint32_t *) vaddr), UNIQ_VAL_1);
}


static int simple_dfault_happend = 0;

/* MMU data access exception handler */
static inline int simple_dfault_handler(uint32_t trap_nr, void *data) {
	simple_dfault_happend = 1;
	return 0;
}



TEST_CASE("Writing in read-only memory should generate exception."
		  " Vaule must left untouched.") {
	uint32_t addr = UNIQ_VAL_1;
	uint32_t vaddr = VADDR(BIGADDR, &addr, PAGE_SIZE);

	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DATA_SECUR_FAULT, simple_dfault_handler);

	vmem_map_region((mmu_ctx_t)0, (mmu_paddr_t)(&addr) & ~(PAGE_SIZE - 1), BIGADDR, PAGE_SIZE,
				VMEM_PAGE_CACHEABLE | VMEM_PAGE_EXECUTABLE);

	/* Data access exception */
	*((volatile uint32_t *) vaddr) = UNIQ_VAL_2;

	test_assert(addr == UNIQ_VAL_1);
	test_assert(simple_dfault_happend);
}

#endif


/* MMU data access exception handler */
static inline int dfault_handler(uint32_t trap_nr, void *data) {
	vmem_page_set_flags((mmu_ctx_t) 0x0, BIGADDR1, VMEM_PAGE_WRITABLE);
	/* repeat instruction */
	return 1;
}

TEST_CASE("Writing to read-only memory should cause exception."
		 " Exception handler should allow write.") {
	uint32_t addr = UNIQ_VAL_1;
	uint32_t vaddr = VADDR(BIGADDR1, &addr, PAGE_SIZE);

	mmu_on();

	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DATA_SECUR_FAULT, dfault_handler);

	vmem_map_region((mmu_ctx_t)0, (mmu_paddr_t)&addr & ~(PAGE_SIZE - 1), BIGADDR1, PAGE_SIZE,
			VMEM_PAGE_CACHEABLE);

	*((volatile uint32_t *) vaddr) = UNIQ_VAL_1;
	test_assert_equal(addr, UNIQ_VAL_1);

	mmu_off();
}


static uint8_t __attribute__ ((aligned (PAGE_SIZE))) page[PAGE_SIZE];
static int flag = 0;

static inline int pagefault_handler(uint32_t nr, void *data) {
	int _cr2, err_addr;
	asm ("mov %%cr2, %0":"=r" (_cr2):);
	err_addr = _cr2 & ~(PAGE_SIZE - 1);

	vmem_map_region((mmu_ctx_t) 0, (mmu_paddr_t) ((unsigned long) &page),
			(mmu_vaddr_t) err_addr, PAGE_SIZE, VMEM_PAGE_CACHEABLE |
			VMEM_PAGE_WRITABLE);

	flag = 1;
	return 1; // execute exception-cause instruction once more
}

TEST_CASE("Pagefault should be considered right.") {
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DATA_MISS_FAULT,
				&pagefault_handler); //data mmu miss

	mmu_on();

	*((uint32_t *) BIGADDR2) = UNIQ_VAL_1;

	test_assert_equal(*((unsigned long *) BIGADDR2), UNIQ_VAL_1);

	mmu_off();
}

static int mmu_case_setup(void) {
	return 0;
}

static int mmu_case_teardown(void) {
	return 0;
}

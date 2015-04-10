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
#include <hal/ipl.h>
#include <hal/test/traps_core.h>

EMBOX_TEST_SUITE("Complex MMU core support test suite");

TEST_SETUP(mmu_case_setup);
TEST_TEARDOWN(mmu_case_teardown);

static mmu_ctx_t ctx;
static volatile int exception_flag;

static char page[VMEM_PAGE_SIZE] __attribute__((aligned(VMEM_PAGE_SIZE)));
//static char PAGE2[VMEM_PAGE_SIZE];

static mmu_paddr_t paddr = (mmu_paddr_t) page;

/*mapping to address, assuming there is no mem at here */
#define BIGADDR 0xf0010000

/* Unique values that seems to pretty to accidentally happen in mem,
 * used to read-n-check */
#define UNIQ_VAL 0x12345678

/* MMU data access exception handler */
static inline int dfault_handler(uint32_t trap_nr, void *data) {
	vmem_page_set_flags((mmu_ctx_t) ctx, BIGADDR, VMEM_PAGE_WRITABLE);
	exception_flag = 1;

	test_assert_not_equal(*((volatile uint32_t *) paddr), UNIQ_VAL);

	/* repeat instruction */
	return 1;
}

TEST_CASE("Writing to read-only memory should cause exception."
		 " Exception handler should allow write.") {
	mmu_vaddr_t vaddr = (mmu_vaddr_t) BIGADDR;
	exception_flag = 0;

	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DATA_SECUR_FAULT, dfault_handler);

	vmem_map_region(ctx, paddr, BIGADDR, VMEM_PAGE_SIZE, 0);

	*((volatile uint32_t *) vaddr) = UNIQ_VAL; /* <- exception */

	test_assert_equal(exception_flag, 1);

	test_assert_equal(*((volatile uint32_t *) vaddr), UNIQ_VAL);

	vmem_unmap_region(ctx, BIGADDR, VMEM_PAGE_SIZE, 0);
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DATA_SECUR_FAULT, NULL);
}

static inline int pagefault_handler(uint32_t nr, void *data) {
	int err_addr;

	err_addr = mmu_get_fault_address();

	vmem_map_region(ctx, (mmu_paddr_t) ((unsigned long) page),
			(mmu_vaddr_t) err_addr, VMEM_PAGE_SIZE, VMEM_PAGE_WRITABLE);

	exception_flag = 1;
	return 1; // execute exception-cause instruction once more
}

TEST_CASE("Pagefault should be considered right.") {
	exception_flag = 0;

	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DATA_MISS_FAULT,
				&pagefault_handler); //data mmu miss

	*((uint32_t *) BIGADDR) = UNIQ_VAL; /* <- exception */

	test_assert_equal(exception_flag, 1);

	test_assert_equal(*((unsigned long *) BIGADDR), UNIQ_VAL);

	vmem_unmap_region(ctx, BIGADDR, VMEM_PAGE_SIZE, 0);
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, MMU_DATA_MISS_FAULT, NULL);
}

/* TODO: Remove this. */
//static ipl_t ipl;

static int mmu_case_setup(void) {
#if 0
	ipl = ipl_save();

	vmem_off();
	test_assert_zero(vmem_create_context(&ctx));
	test_assert_zero(vmem_init_context(&ctx));
#endif
	ctx = vmem_current_context();
	vmem_set_context(ctx);

	if (!vmem_mmu_enabled()) {
		vmem_on();
	}

	memset(page, 0, sizeof page);

	return 0;
}

static int mmu_case_teardown(void) {
	if (vmem_mmu_enabled()) {
		vmem_off();
	}
#if 0
	vmem_free_context(ctx);
	ipl_restore(ipl);
#endif
	return 0;
}

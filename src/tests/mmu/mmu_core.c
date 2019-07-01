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

/* TODO repalce this buffer with page_alloc(). Page size could be like 1MB,
 * and such static variable will be a bad thing */
static char mmu_test_buffer[2 * VMEM_PAGE_SIZE];
static char *page;
static mmu_paddr_t paddr;

/*mapping to address, assuming there is no mem at here */
#define BIGADDR (0xf0111111 & ~MMU_PAGE_MASK)

/* Unique values that seems to pretty to accidentally happen in mem,
 * used to read-n-check */
#define UNIQ_VAL 0x12345678

/* MMU data access exception handler */
static inline int dfault_handler(uint32_t trap_nr, void *data) {
	vmem_set_flags((mmu_ctx_t) ctx, BIGADDR, VMEM_PAGE_SIZE, PROT_WRITE | PROT_READ );
	exception_flag = 1;

	test_assert_not_equal(*((volatile uint32_t *) paddr), UNIQ_VAL);

	/* repeat instruction */
	return 1;
}

static inline int pagefault_handler(uint32_t nr, void *data) {
	int err_addr;

	err_addr = mmu_get_fault_address();

	vmem_map_region(ctx, (mmu_paddr_t) ((unsigned long) page),
			(mmu_vaddr_t) err_addr, VMEM_PAGE_SIZE, PROT_WRITE | PROT_READ );

	exception_flag = 1;
	return 1; // execute exception-cause instruction once more
}

TEST_CASE("Pagefault should be considered right.") {
	exception_flag = 0;

	set_fault_handler(MMU_DATA_MISS, &pagefault_handler); //data mmu miss

	*((uint32_t *) BIGADDR) = UNIQ_VAL; /* <- exception */

	test_assert_equal(exception_flag, 1);

	test_assert_equal(*((unsigned long *) BIGADDR), UNIQ_VAL);

	set_fault_handler(MMU_DATA_MISS, NULL);
}

static inline int readonly_handler(uint32_t nr, void *data) {
	exception_flag = 1;

	/* Remap page with write access */
	vmem_unmap_region(ctx, (uintptr_t) page, VMEM_PAGE_SIZE);
	vmem_map_region(ctx, (uintptr_t) page, (uintptr_t) page, VMEM_PAGE_SIZE, PROT_WRITE | PROT_READ );

	return 1;
}

TEST_CASE("Readonly pages shouldn't be written") {
	exception_flag = 0;

	set_fault_handler(MMU_DATA_MISS, &readonly_handler); //data mmu miss

	vmem_map_region(ctx, (uintptr_t) page, (uintptr_t) page, VMEM_PAGE_SIZE, 0);
	mmu_drop_privileges();
	*page += 1;

	test_assert_equal(exception_flag, 1);
	test_assert_equal(*page, 1);

	vmem_unmap_region(ctx, (mmu_vaddr_t) page, VMEM_PAGE_SIZE);

	set_fault_handler(MMU_DATA_MISS, NULL);
}

static int mmu_case_setup(void) {
	ctx = vmem_current_context();
	mmu_set_context(ctx);

	/* XXX hack for page-aligned array. __aligned__ don't work for big pages */
	page = (char*) (((uintptr_t) mmu_test_buffer + VMEM_PAGE_SIZE - 1) &
			(~MMU_PAGE_MASK));
	paddr = (mmu_paddr_t) page;
	memset(page, 0, VMEM_PAGE_SIZE);

	vmem_unmap_region(ctx, (mmu_vaddr_t) page, VMEM_PAGE_SIZE);

	return 0;
}

static int mmu_case_teardown(void) {
	mmu_sys_privileges();
	vmem_map_region(ctx, (mmu_vaddr_t) page, (mmu_vaddr_t) page, VMEM_PAGE_SIZE, PROT_WRITE | PROT_READ );
	return 0;
}

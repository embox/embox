/**
 * @file
 * @brief MMU test suite for complex testing mmu core support
 *
 * @date 28.07.11
 * @author Anton Kozlov
 */
#include <string.h>

#include <embox/test.h>
#include <framework/test/assert.h>

#include <mem/vmem.h>
#include <hal/ipl.h>
#include <hal/test/traps_core.h>

EMBOX_TEST_SUITE("MMU translate address test");

TEST_SETUP(mmu_case_setup);
TEST_TEARDOWN(mmu_case_teardown);

static volatile char test_page[VMEM_PAGE_SIZE] __attribute__((aligned(VMEM_PAGE_SIZE)));
static mmu_ctx_t ctx;

#define TEST_VIRT_ADDR_MAP (test_page + OPTION_GET(NUMBER,vaddr_offset))

TEST_CASE("Double map phy_page to different address") {
	int i;

	for(i = 0; i < sizeof(test_page); i++ ) {
		test_page[i] = (char)i & 0x7F;
	}
	for(i = 0; i < sizeof(test_page); i++ ) {
		test_assert_equal(TEST_VIRT_ADDR_MAP[i], (char)i & 0x7F);
	}
}

static int mmu_case_setup(void) {
	int res;

	ctx = vmem_current_context();

	res = vmem_map_region(ctx, (mmu_vaddr_t) test_page,
			(mmu_vaddr_t) TEST_VIRT_ADDR_MAP, VMEM_PAGE_SIZE, PROT_WRITE | PROT_READ);

	if (res) {
		return res;
	}
	memset((void*)test_page, 0, VMEM_PAGE_SIZE);

	return 0;
}

static int mmu_case_teardown(void) {
	vmem_unmap_region(ctx, (mmu_vaddr_t) TEST_VIRT_ADDR_MAP, VMEM_PAGE_SIZE);

	return 0;
}

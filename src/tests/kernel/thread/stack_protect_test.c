#include <stdio.h>
#include <pthread.h>

#include <mem/vmem.h>
#include <hal/mmu.h>
#include <hal/test/traps_core.h>
#include <kernel/thread/stack_protect.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("Stack protection test suite");

TEST_SETUP(case_setup);
TEST_TEARDOWN(case_teardown);

static int mmu_was_enabled;
static mmu_ctx_t ctx;
static volatile int exception_flag;
static volatile int first_check;
static volatile int second_check;
static mmu_vaddr_t page;


static inline int soverflow_handler(uint32_t nr, void *data) {
	exception_flag = 1;

	vmem_unmap_region(ctx, page, VMEM_PAGE_SIZE);
	vmem_map_region(ctx, page, page, VMEM_PAGE_SIZE, VMEM_PAGE_WRITABLE);

	return 1;
}

void *test_func(void *unused) {
    char c[1];
    c[-7000] = 1;
    first_check = c[-7000];   // should be OK
    page = (mmu_vaddr_t)(c - 9000) & ~VMEM_PAGE_MASK;
    c[-9000] = 1;
    second_check = exception_flag;
    return NULL;
}

TEST_CASE("Stack overflow causes exception") {
	exception_flag = 0;
    first_check = 0;
    second_check = 0;
	set_fault_handler(MMU_DATA_MISS, &soverflow_handler); //data mmu miss

    pthread_t thread;

    assert(!pthread_create(&thread, NULL, test_func, NULL));

    assert(!pthread_join(thread, NULL));

    test_assert_equal(exception_flag, 1);
	test_assert_equal(first_check, 1);
    test_assert_equal(second_check, 1);


	set_fault_handler(MMU_DATA_MISS, NULL);
}

static int case_setup(void) {
    ctx = vmem_current_context();
    vmem_set_context(ctx);

    mmu_was_enabled = vmem_mmu_enabled();
    if (!mmu_was_enabled) {
        vmem_on();
    }
    stack_protect_enable();
    return 0;
}

static int case_teardown(void) {
    if (vmem_mmu_enabled() && !mmu_was_enabled) {
        vmem_off();
    }
    stack_protect_disable();
    return 0;
}

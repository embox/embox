#include <assert.h>
#include <stddef.h>

#include <kernel/thread.h>
#include <kernel/thread/stack_protect.h>

#include <mem/vmem.h>
#include <mem/vmem/vmem_alloc.h>

static int enabled = 0;

void stack_protect(struct thread *t, size_t size)
{
    mmu_ctx_t ctx;

    if (!stack_protect_enabled()) { return; }

    assert(((mmu_vaddr_t)t) & ~VMEM_PAGE_MASK);

    ctx = vmem_current_context();
    vmem_set_context(ctx);

    vmem_unmap_region(ctx, (mmu_vaddr_t)t, size, 0);
    vmem_map_region(ctx, (mmu_vaddr_t)t, (mmu_vaddr_t)t, size, VMEM_PAGE_WRITABLE | VMEM_PAGE_USERMODE);

    vmem_page_set_flags(ctx, (mmu_vaddr_t)t + VMEM_PAGE_SIZE, 0);
    mmu_flush_tlb();
}

void stack_protect_release(struct thread *t)
{
    mmu_ctx_t ctx;

    if (!stack_protect_enabled()) { return; }

    ctx = vmem_current_context();
    vmem_set_context(ctx);
    vmem_page_set_flags(ctx, (mmu_vaddr_t)t + VMEM_PAGE_SIZE, VMEM_PAGE_WRITABLE | VMEM_PAGE_USERMODE);
    mmu_flush_tlb();
}

void stack_protect_enable() {
    enabled = 1;
}

void stack_protect_disable() {
    enabled = 0;
}

int stack_protect_enabled()
{
    return enabled;
}

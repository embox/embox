#include <xen/elfnote.h>

.section ".note", "a"

#define ELFNOTE(type, desc) \
    .p2align 2; \
    .long 1f - 0f; \
    .long 3f - 2f; \
    .long type; \
0:  .asciz "Xen"; \
1:  .p2align 2; \
2:   desc; \
3:  .p2align 2;

ELFNOTE(XEN_ELFNOTE_XEN_VERSION, .asciz "xen-3.0")
ELFNOTE(XEN_ELFNOTE_LOADER, .asciz "generic")
ELFNOTE(XEN_ELFNOTE_PAE_MODE, .asciz "yes")

ELFNOTE(XEN_ELFNOTE_VIRT_BASE, .long 0x100000)
ELFNOTE(XEN_ELFNOTE_PADDR_OFFSET, .long 0x100000)
ELFNOTE(XEN_ELFNOTE_HYPERCALL_PAGE, .long 0x100000)

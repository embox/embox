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

ELFNOTE(0x8, .asciz "generic")
ELFNOTE(0x5, .asciz "xen-3.0")
ELFNOTE(0x9, .asciz "yes,bimodal")

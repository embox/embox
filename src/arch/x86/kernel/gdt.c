/**
 * @file
 * @brief Global Descriptor Table (GDT)
 * @details The IDT is used by the processor to determine
 *          used to define the characteristics of the various
 *          memory areas used during program execution.
 * @date 22.12.10
 * @author Nikolay Korotky
 */

#include <types.h>
#include <asm/regs.h>
#include <asm/traps.h>
#include <kernel/panic.h>

#if 0
#define BASE_TSS       0x08
#define KERNEL_CS      0x10
#define KERNEL_DS      0x18
#define DBF_TSS        0x20

#define ACC_TSS        0x09
#define ACC_TSS_BUSY   0x02
#define ACC_CODE_R     0x1A
#define ACC_DATA_W     0x12
#define ACC_PL_K       0x00
#define ACC_P          0x80
#define SZ_32          0x4
#define SZ_16          0x0
#define SZ_G           0x8

#define GDT_SIZE       (0x28 / 8)

typedef struct gate_init_entry {
        uint32_t  entrypoint;
        uint16_t  vector;
        uint16_t  type;
} gate_init_entry_t;
#endif

#if 0
typedef struct x86_desc {
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t  base_med;
        uint8_t  access;
        uint8_t  limit_high:4;
        uint8_t  granularity:4;
        uint8_t  base_high;
} x86_desc_t __attribute__((packed));

typedef struct x86_tss {
        uint32_t back_link;
        uint32_t esp0, ss0;
        uint32_t esp1, ss1;
        uint32_t esp2, ss2;
        uint32_t cr3;
        uint32_t eip, eflags;
        uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
        uint32_t es, cs, ss, ds, fs, gs;
        uint32_t ldt;
        uint16_t trace_trap;
        uint16_t io_bit_map_offset;
} x86_tss_t;

typedef struct ps_desc {
        uint16_t pad;
        uint16_t limit;
        uint32_t linear_base;
} ps_desc_t;

#define SET_GDT(pseudo_desc)                 \
        __asm__ __volatile__(                \
                "lgdt %0" :                  \
                : "m"((pseudo_desc)->limit), \
                "m"(*pseudo_desc)            \
        )

#define SET_TR(seg)                         \
        __asm__ __volatile__(               \
                "ltr %0"                    \
                : : "rm"((uint16_t) (seg)) \
        )
#endif

#if 0
static x86_tss_t         base_tss;
static x86_desc_t        base_gdt[GDT_SIZE];

static void      handle_dbf(void);
static char      dbf_stack[2048];
static x86_tss_t dbf_tss = {
        0/*back_link*/,
        0/*esp0*/, 0/*ss0*/, 0/*esp1*/, 0/*ss1*/, 0/*esp2*/, 0/*ss2*/,
        0/*cr3*/,
        (uint32_t) handle_dbf/*eip*/, 0x00000082/*eflags*/,
        0/*eax*/, 0/*ecx*/, 0/*edx*/, 0/*ebx*/,
        (uint32_t) dbf_stack + sizeof(dbf_stack)/*esp*/,
        0/*ebp*/, 0/*esi*/, 0/*edi*/,
        KERNEL_DS/*es*/, KERNEL_CS/*cs*/, KERNEL_DS/*ss*/,
        KERNEL_DS/*ds*/, KERNEL_DS/*fs*/, KERNEL_DS/*gs*/,
        0/*ldt*/, 0/*trace_trap*/, 0x8000/*io_bit_map_offset*/
};

static inline void fill_descriptor(x86_desc_t *desc, uint32_t base,
                uint32_t limit, uint8_t access, uint8_t sizebits) {
        if (limit > 0xfffff) {
                limit >>= 12;
                sizebits |= SZ_G;
        }
        desc->limit_low   = limit & 0xffff;
        desc->base_low    = base & 0xffff;
        desc->base_med    = (base >> 16) & 0xff;
        desc->access      = access | ACC_P;
        desc->limit_high  = limit >> 16;
        desc->granularity = sizebits;
        desc->base_high   = base >> 24;
}
#endif

void gdt_init(void) {
#if 0
        ps_desc_t pdesc;
        /* Initialize GDT */
        /* Initialize the base TSS descriptor. */
        fill_descriptor(&base_gdt[BASE_TSS / 8], (uint32_t *) &base_tss,
                        sizeof(base_tss) - 1, ACC_PL_K | ACC_TSS, 0);
        /* Initialize the TSS descriptor for the double fault handler */
        fill_descriptor(&base_gdt[DBF_TSS / 8], (uint32_t *) &dbf_tss,
                        sizeof(dbf_tss) - 1, ACC_PL_K | ACC_TSS, 0);
        /* Initialize the 32-bit kernel code and data segment descriptors
           to point to the base of the kernel linear space region. */
        fill_descriptor(&base_gdt[KERNEL_CS / 8], 0, 0xffffffff,
                        ACC_PL_K | ACC_CODE_R, SZ_32);
        fill_descriptor(&base_gdt[KERNEL_DS / 8], 0, 0xffffffff,
                        ACC_PL_K | ACC_DATA_W, SZ_32);
        /* Load GDT (Global Descriptor Table) */
        pdesc.limit = sizeof(base_gdt) - 1;
        pdesc.linear_base = (uint32_t *) &base_gdt;
        SET_GDT(&pdesc);
        __asm__ __volatile__(
                "ljmp  %0,$1f ;  1:"
                : : "i" (KERNEL_CS)
        );
        __asm__ __volatile__(
                "movw %w0,%%ds"
                : : "r" (KERNEL_DS)
        );
        __asm__ __volatile__(
                "movw %w0,%%es"
                : : "r" (KERNEL_DS)
        );
        __asm__ __volatile__(
                "movw %w0,%%ss"
                : : "r" (KERNEL_DS)
        );
        __asm__ __volatile__(
                "movw %w0,%%fs"
                : : "r" (0)
        );
        __asm__ __volatile__(
                "movw %w0,%%gs"
                : : "r" (0)
        );
        /* Load TSS (Task State Segment) */
        base_gdt[BASE_TSS / 8].access &= ~ACC_TSS_BUSY;
        SET_TR(BASE_TSS);
#endif
}


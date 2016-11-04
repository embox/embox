#include <stdint.h>
#include <xen/xen.h>

#include <xen_hypercall-x86_32.h>

/*
 * These are assembler stubs in entry.S.
 * They are the actual entry points for virtual exceptions.
 */
void divide_error(void);
void debug(void);
void int3(void);
void overflow(void);
void bounds(void);
void invalid_op(void);
void device_not_available(void);
void coprocessor_segment_overrun(void);
void invalid_TSS(void);
void segment_not_present(void);
void stack_segment(void);
void general_protection(void);
void page_fault(void);
void coprocessor_error(void);
void simd_coprocessor_error(void);
void alignment_check(void);
void spurious_interrupt_bug(void);
void machine_check(void);

/* Dummy implementation.  Should actually do something */
void do_divide_error(void) {}
void do_debug(void) {}
void do_int3(void) {}
void do_overflow(void) {}
void do_bounds(void) {}
void do_invalid_op(void) {}
void do_device_not_available(void) {}
void do_coprocessor_segment_overrun(void) {}
void do_invalid_TSS(void) {}
void do_segment_not_present(void) {}
void do_stack_segment(void) {}
void do_general_protection(void) {}
void do_page_fault(void) {}
void do_coprocessor_error(void) {}
void do_simd_coprocessor_error(void) {}
void do_alignment_check(void) {}
void do_spurious_interrupt_bug(void) {}
void do_machine_check(void) {}

/*
 * Submit a virtual IDT to teh hypervisor. This consists of tuples
 * (interrupt vector, privilege ring, CS:EIP of handler).
 * The 'privilege ring' field specifies the least-privileged ring that
 * can trap to that vector using a software-interrupt instruction (INT).
 */
static trap_info_t trap_table[] = {
    {  0, 0, FLAT_KERNEL_CS, (unsigned long)divide_error                },
    {  1, 0, FLAT_KERNEL_CS, (unsigned long)debug                       },
    {  3, 3, FLAT_KERNEL_CS, (unsigned long)int3                        },
    {  4, 3, FLAT_KERNEL_CS, (unsigned long)overflow                    },
    {  5, 3, FLAT_KERNEL_CS, (unsigned long)bounds                      },
    {  6, 0, FLAT_KERNEL_CS, (unsigned long)invalid_op                  },
    {  7, 0, FLAT_KERNEL_CS, (unsigned long)device_not_available        },
    {  9, 0, FLAT_KERNEL_CS, (unsigned long)coprocessor_segment_overrun },
    { 10, 0, FLAT_KERNEL_CS, (unsigned long)invalid_TSS                 },
    { 11, 0, FLAT_KERNEL_CS, (unsigned long)segment_not_present         },
    { 12, 0, FLAT_KERNEL_CS, (unsigned long)stack_segment               },
    { 13, 0, FLAT_KERNEL_CS, (unsigned long)general_protection          },
    { 14, 0, FLAT_KERNEL_CS, (unsigned long)page_fault                  },
    { 15, 0, FLAT_KERNEL_CS, (unsigned long)spurious_interrupt_bug      },
    { 16, 0, FLAT_KERNEL_CS, (unsigned long)coprocessor_error           },
    { 17, 0, FLAT_KERNEL_CS, (unsigned long)alignment_check             },
    { 19, 0, FLAT_KERNEL_CS, (unsigned long)simd_coprocessor_error      },
    {  0, 0,           0, 0                           }
};

void trap_init(void)
{
    HYPERVISOR_set_trap_table(trap_table);    
}


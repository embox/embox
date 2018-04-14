
#include <stdint.h>
#include <xen/xen.h>
#include <xen/event.h> // pt_regs
#include <kernel/printk.h>
#include <kernel/panic.h>
#include <kernel/irq.h>

extern shared_info_t xen_shared_info;

#if defined(__i386__)
#include <xen_hypercall-x86_32.h>
/*
#elif defined(__x86_64__)
#include <hypercall-x86_64.h>
#elif defined(__arm__) || defined(__aarch64__)
#include <hypercall-arm.h>
*/
#else
#error "Unsupported architecture"
#endif

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
void do_divide_error(struct pt_regs *regs, int error_code) {
	panic("DIVIDE ERROR called\n");
}
void do_debug(void) {
	panic("ERROR_1\n");
}
void do_int3(void) {
	panic("ERROR_2\n");
}
void do_overflow(void) {
	panic("ERROR_3\n");
}
void do_bounds(void) {
	panic("ERROR_4\n");
}
void do_invalid_op(void) {
	panic("ERROR_5\n");
}
void do_device_not_available(void) {
	panic("ERROR_6\n");
}
void do_coprocessor_segment_overrun(void) {
	panic("ERROR_7\n");
}
void do_invalid_TSS(void) {
	panic("ERROR_8\n");
}
void do_segment_not_present(void) {
	panic("ERROR_9\n");
}
void do_stack_segment(void) {
	panic("ERROR_10\n");
}
void do_general_protection(void) {
	panic("ERROR_11\n");
}
void do_page_fault(void) {
	panic("ERROR_12\n");
}
void do_coprocessor_error(void) {
	panic("ERROR_13\n");
}
void do_simd_coprocessor_error(void) {
	panic("ERROR_14\n");
}
void do_alignment_check(void) {
	panic("ERROR_15\n");
}
void do_spurious_interrupt_bug(void) {
	panic("ERROR_16\n");
}
void do_machine_check(void) {
}

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

/* Assembler interface fns in entry.S. */
void hypervisor_callback(void);
void failsafe_callback(void);

void trap_init(void)
{
    HYPERVISOR_set_trap_table(trap_table);

#ifdef __i386__
    HYPERVISOR_set_callbacks(
		    FLAT_KERNEL_CS, (unsigned long)hypervisor_callback,
		    FLAT_KERNEL_CS, (unsigned long)failsafe_callback);
#else
    HYPERVISOR_set_callbacks(
		    (unsigned long)hypervisor_callback,
		    (unsigned long)failsafe_callback, 0);
#endif
}

/**
 * @file
 *
 * @date 17.12.10
 * @author Nikolay Korotky
 */

#include <asm/traps.h>
#include <asm/entry.h>

	.text

IRQ_ENTRY(0) /* system timer (cannot be changed) */
IRQ_ENTRY(1) /* keyboard controller (cannot be changed) */
IRQ_ENTRY(2) /* cascaded signals from IRQs 8–15 */
IRQ_ENTRY(3) /* serial port controller for COM2(shared with COM4, if present) */
IRQ_ENTRY(4) /* serial port controller for COM1(shared with COM3, if present) */
IRQ_ENTRY(5) /* LPT port 2  or  sound card */
IRQ_ENTRY(6) /* floppy disk controller */
IRQ_ENTRY(7) /* LPT port 1 or sound card(8-bit Sound Blaster and compatibles) */

IRQ_ENTRY(8)  /* real-time clock */
IRQ_ENTRY(9)  /* open interrupt/available or SCSI host adapter */
IRQ_ENTRY(10) /* open interrupt / available  or  SCSI  or  NIC */
IRQ_ENTRY(11) /* open interrupt / available  or  SCSI  or  NIC */
IRQ_ENTRY(12) /* mouse on PS/2 connector */
IRQ_ENTRY(13) /* math co-processor/integrated FPU/inter-processor interrupt(use depends on OS) */
IRQ_ENTRY(14) /* primary ATA channel */
IRQ_ENTRY(15) /* secondary ATA channel */

	.section .traps.text,"x"

irq_stub:
	SAVE_ALL               /* Entry code is already in higher address */

	mov     %esp, %eax     /* Used for fastcall of handler */
	call    irq_handler

	RESTORE_ALL

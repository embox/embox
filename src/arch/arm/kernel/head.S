/*
 *  start.S
 *  very simple code for ARM 7TDMI
 *
 *  author: SU Hang
 *  date:   2004-08-28
 */


#define MODE_SVC 0x13
#define I_BIT   0x80

.text
	.align 4
	.global begin
	.type begin, function

begin:

//dissable interupts
	/*disable I-bit*/
	mov     r0, #I_BIT|MODE_SVC
	msr     cpsr_c, r0
//copy data, reloc section
//zero bss
//set stack pointer
//hardware_init_hook();(if zero not to call)
//soft_init_hook(); (also)
//main();
//while (true);
	mov	r1,#0xd2
	sub	r2,r0,r1
	ldr 	sp, =irq_stack           @ set sp_irq = irq_stack
	bl	hello
	b	begin

.data
	.align  4
irq_stack:
	.space	4096


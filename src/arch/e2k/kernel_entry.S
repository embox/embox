/**
 * @file
 * @brief
 *
 * @date Mar 13, 2018
 * @author Anton Bondarev
 */

/*
 * Trap table entries implemented on assembler
 */

//#include <hal/cpu_regs.h>
//#include <hal/e2k_api.h>
//#include <arch/cpu.h>
//#include <sys/offsets.h>

.global	cpuVecHandlerNorm;
.global ttable_entry0;

#define cpuVA_MODDATA 0x0
#define modPRIVATE 0x0
#define SIZEOF_CORE 0x10

.section .ttable_entry0, "ax",@progbits
.align 8
.type ttable_entry0,@function
ttable_entry0:

/* kernel */
	{
	ipd 2
	disp	%ctpr1, cpuVecHandlerNorm;
	rrd	%lsr, %dr16
	addd %dr0, %dr1, %dr0           // dr0 = pModData->mdCores[cpuCoreGet()]
	}
	{
	ct	%ctpr1;				  // kernel_trap_handler()
	}
.size	 $ttable_entry0,.-$ttable_entry0




.global ttable_entry1;

.section .ttable_entry1, "ax",@progbits
.align 8
.type ttable_entry1,@function
ttable_entry1:
	{
	setwd wsz = 0x4
	disp %ctpr1, cpuSyscall
	}
	{
	puttagd %dr0, 0, %dr0
	puttagd %dr1, 0, %dr1
	}
	{
	puttagd %dr2, 0, %dr2
	puttagd %dr3, 0, %dr3
	}
	{
	puttagd %dr4, 0, %dr4
	ct %ctpr1
	}
.size	 $ttable_entry1,.-$ttable_entry1



.global ttable_entry2;

.section .ttable_entry2, "ax",@progbits
.align 8
.type ttable_entry2,@function
ttable_entry2:
#if 0
	{
	setwd wsz = 0x4
	disp %ctpr1, cpuJmpSyscall
	}
#endif
	{
	puttagd %dr0, 0, %dr0
	puttagd %dr1, 0, %dr1
	}
	{
	puttagd %dr2, 0, %dr2
	puttagd %dr3, 0, %dr3
	}
	{
	puttagd %dr4, 0, %dr4
	ct %ctpr1
	}
.size	 $ttable_entry2,.-$ttable_entry2

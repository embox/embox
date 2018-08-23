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

.global ttable_entry0;

.section .ttable_entry0, "ax",@progbits
.align 8
.type ttable_entry0,@function
ttable_entry0:
	{
	setwd   wsz = 13, nfx = 1;
	}
	{
	rrd     %ctpr1, %dr6;
	}
	{
	rrd     %ctpr2, %dr5;
	}
	{
	rrd     %ctpr3, %dr3;

	ipd 0
	}

	/* kernel */
	{
	ipd 1
	}
	{
	ipd 2
	disp    %ctpr1, cpuVecHandlerNorm;
	}
	{
	ct      %ctpr1;
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
	{
	setwd wsz = 0x4
	disp %ctpr1, cpuJmpSyscall
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
.size	 $ttable_entry2,.-$ttable_entry2

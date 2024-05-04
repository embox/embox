/**
 * @file
 *
 * @date 19.12.2016
 * @author Anton Bondarev
 */

#include <asm/asm.h>
#include <asm/regdef.h>

#define RALINK_SYSCTL_BASE 0xB0000000
#define SDRAM_CFG0_REG     RALINK_SYSCTL_BASE + 0x0300
#define SDRAM_CFG1_REG     RALINK_SYSCTL_BASE + 0x0304

#define SDRAM_CFG1_SDRAM_INIT_START (1u << 31)
#define SDRAM_CFG1_SDRAM_INIT_DONE  (1u << 30)

#define ON_BOARD_16M_DRAM_COMPONENT y
#define ON_BOARD_16BIT_DRAM_BUS     y
#define DELAY_USEC(us) ((500*(us))/3)

	.section    .text
	.align  4
	.global hardware_init_hook

hardware_init_hook:
# Initialize Misc. Cop0 state
#if 0
	# Read status register
	mfc0	$10, $12
	# Set up Status register:
	# Disable Coprocessor Usable bits
	# Turn off Reduce Power bit
	# Turn off reverse endian
	# Turn off BEV (use normal exception vectors)
	# Clear TS, SR, NMI bits
	# Clear Interrupt masks
	# Clear User Mode
	# Clear ERL
	# Set EXL
	# Clear Interrupt Enable
	# modify by Bruce
	#li	$11, 0x0000ff02
	li	$11, 0x00000004
	mtc0	$11, $12

	# Disable watch exceptions
	mtc0	$0, $18

	# Clear Watch Status bits
	li	$11, 0x3
	mtc0	$11, $19

	# Clear WP bit to avoid watch exception upon user code entry
	# Clear IV bit - Interrupts go to general exception vector
	# Clear software interrupts
	mtc0	$0, $13

	# Set KSeg0 to cacheable
	# Config.K0
	mfc0	$10, $16
	li	$11, 0x7
	not	$11
	and	$10, $11
	or	$10, 0x3
	mtc0	$10, $16

	# Clear Count register
	mtc0	$0, $9

	# Set compare to -1 to delay 1st count=compare
	# Also, clears timer interrupt
	li	$10, -1
	mtc0	$10, $11

	# Cache initialization routine
	# Long and needed on HW
	# Can be skipped if using magic simulation cache flush

	# Determine how big the I$ is
/*
 ************************************************************************
 *         C O N F I G 1   R E G I S T E R   ( 1 6, SELECT 1 )          *
 ************************************************************************
 *
 *  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
 *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |M|  MMU Size |  IS |  IL |  IA |  DS |  DL |  DA |Rsvd |W|C|E|F| Config1
 * | |           |     |     |     |     |     |     |     |R|A|P|P|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
	mfc0	$10, $16, 1		# .word 0x400a8001

	# Isolate I$ Line Size
	sll	$11, $10, 10
	srl	$11, 29

	# Skip ahead if No I$
	beq	$11, $0, 10f
	nop

	li	$14, 2
	sllv	$11, $14, $11		# Now have true I$ line size in bytes

	sll	$12, $10, 7
	srl	$12, 29
	li	$14, 64
	sllv	$12, $14, $12		# I$ Sets per way

	sll	$13, $10, 13
	srl	$13, 29			# I$ Assoc (-1)
	add	$13, 1
	mul	$12, $12, $13		# Total number of sets

	lui	$14, 0x8000		# Get a KSeg0 address for cacheops

	# Clear TagLo/TagHi registers
	mtc0	$0, $28
	mtc0	$0, $29

	move	$15, $12

	# Index Store Tag Cache Op
	# Will invalidate the tag entry, clear the lock bit, and clear the LRF bit
1:	cache	0x8, 0($14)
	add	$15, -1			# Decrement set counter

	bne	$15, $0, 1b
	add	$14, $11		# Get next line address

	# Now go through and invalidate the D$
	# Now that the I$ has been flushed, the rest of the code can be
	# moved to kseg0 and run from the cache to go faster
10:


	# Isolate D$ Line Size
	sll	$11, $10, 19
	srl	$11, 29

	# Skip ahead if No D$
	beq	$11, $0, 10f
	nop

	li	$14, 2
	sllv	$11, $14, $11		# Now have true D$ line size in bytes

	sll	$12, $10, 16
	srl	$12, 29
	li	$14, 64
	sllv	$12, $14, $12		# D$ Sets per way

	sll	$13, $10, 22
	srl	$13, 29			# D$ Assoc (-1)
	add	$13, 1

	mul	$12, $12, $13		# Get total number of sets

	lui	$14, 0x8000		# Get a KSeg0 address for cacheops

	# Clear TagLo/TagHi registers
	mtc0	$0, $28
	mtc0	$0, $29
	mtc0	$0, $28, 2
	mtc0	$0, $29, 2

	move	$15, $12

	# Index Store Tag Cache Op
	# Will invalidate the tag entry, clear the lock bit, and clear the LRF bit
1:	cache	0x9, 0($14)
	add	$15, -1			# Decrement set counter

	bne	$15, $0, 1b
	add	$14, $11		# Get next line address


	#
	# Now go through and initialize the L2$
10:

	# Check L2 cache size
/*
 ************************************************************************
 *         C O N F I G 2   R E G I S T E R   ( 1 6, SELECT 2 )          *
 ************************************************************************
 *
 *  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
 *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |M| TU  |  TS   |  TL   |  TA   |  SU   |  SS   |  SL   |  SA   | Config2
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

	mfc0	$10, $16, 2

	# Isolate L2$ Line Size
	sll	$11, $10, 24
	srl	$11, 28

	# Skip ahead if No L2$
	beq	$11, $0, 10f
	nop

	li	$14, 2
	sllv	$11, $14, $11		# Now have true L2$ line size in bytes

	# Isolate L2$ Sets per Way
	sll	$12, $10, 20
	srl	$12, 28
	li	$14, 64
	sllv	$12, $14, $12		# D$ Sets per way

	# Isolate L2$ Associativity
	sll	$13, $10, 28
	srl	$13, 28			# D$ Assoc (-1)
	add	$13, 1

	mul	$12, $12, $13		# Get total number of sets

	lui	$14, 0x8000		# Get a KSeg0 address for cacheops

	# Clear L23TagLo/L23TagHi registers
	mtc0	$0, $28, 4
	mtc0	$0, $29, 4

	move	$15, $12

	# L2$ Index Store Tag Cache Op
	# Will invalidate the tag entry, clear the lock bit, and clear the LRF bit
1:	cache	0xB, 0($14)
	add	$15, -1			# Decrement set counter

	bne	$15, $0, 1b
	add	$14, $11		# Get next line address


10:
	# Determine if we have a TLB
	mfc0	$11, $16

	sll	$11, 22
	srl	$11, 29

	li	$15, 0x1	# MT = 1  => TLB

	bne	$11, $15, 15f
	nop

	mfc0	$10, $16, 1			# .word 0x400a8001

	sll	$11, $10, 1
	srl	$11, 26		# Number of TLB entries (-1)

	mtc0	$0, $2		# EntryLo0
	mtc0	$0, $3		# EntryLo1
	mtc0	$0, $5		# PageMask
	mtc0	$0, $6		# Wired

	li	$12, 0x80000000

1:
	mtc0	$11, $0		# Index register
	mtc0	$12, $10	# EntryHi
	ssnop			#.word 0x00000040
	ssnop			#.word 0x00000040
	TLBWI
	add	$12, (2<<13)	# Add 8K to the address to avoid TLB conflict with previous entry

	bne	$11, $0, 1b
	add	$11, -1
15:


	/* SDR and DDR initialization: delay 200us
	 */
	li t0, DELAY_USEC(200+40)
	li t1, 0x1
1:
	sub t0, t0, t1
	bnez t0, 1b
	nop


SDR_INIT:
	/* SDR initialization: SDRAM_CFG0
	 */
	li t5, SDRAM_CFG0_REG
	lw t6, 0(t5)
	nop
	and t6, 0xF0000000

	or t6, 0xD1825272
	nop

	sw t6, 0(t5)
	nop

	li t5, SDRAM_CFG1_REG

/*
 *	Turn on SDRAM RBC (BIT 29 in SDRAM_CFG1, offset 0x4) in RT3052.
 *	  RT2880 RBC bit is Reserved bit, and change the same value for RT2880 and RT3052
 *	  Original 0x81xx0600 -> 0xa1xx0600
 *		by bobtseng, 2008.7.7.
 */
#if defined (ON_BOARD_16M_DRAM_COMPONENT)
	li t6, 0xa0000668
#elif defined (ON_BOARD_64M_DRAM_COMPONENT)
	li t6, 0xa1010600
#elif defined (ON_BOARD_128M_DRAM_COMPONENT)
	li t6, 0xa1110600
#elif defined (ON_BOARD_256M_DRAM_COMPONENT)
	li t6, 0xa1120300
#elif defined (ON_BOARD_512M_DRAM_COMPONENT)
	li t6, 0xa1220600
#elif defined(ON_BOARD_DDR1)||defined(ON_BOARD_DDR2)
#else
	DRAM Component not defined
#endif

#ifdef ON_BOARD_32BIT_DRAM_BUS
	and t6, 0xFEFFFFFF
	or t6, (1<<24)
#elif defined ON_BOARD_16BIT_DRAM_BUS
	and t6, 0xFEFFFFFF
#else
	DRAM bus not defined
#endif



DO_SDRINIT:
	nop

	sw 		t6, 0(t5)
	nop

WAIT_SDRAM_INIT_DOWN:
	lw t6, 0(t5)
	nop
	and  	t6, t6, SDRAM_CFG1_SDRAM_INIT_DONE
	beqz	t6, WAIT_SDRAM_INIT_DOWN
	nop
#endif
	/* return from hw_init_hook */
	j ra

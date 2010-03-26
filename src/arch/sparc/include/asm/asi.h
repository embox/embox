/**
 * @file
 * @brief Address Space Identifier values for the sparc.
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 *
 * @author Paul Hatchman (paul@sfe.com.au)
 *					- Pioneer work for sun4m
 * @author Pete A. Zaitcev <zaitcev@ipmce.su>
 * 					- Joint edition for sun4c+sun4m
 */
#ifndef SPARC_ASI_H
#define SPARC_ASI_H

/* The first batch are for the sun4c. */

#define ASI_NULL1           0x00
#define ASI_NULL2           0x01

/* sun4c and sun4 control registers and mmu/vac ops */
#define ASI_CONTROL         0x02
#define ASI_SEGMAP          0x03
#define ASI_PTE             0x04
#define ASI_HWFLUSHSEG      0x05
#define ASI_HWFLUSHPAGE     0x06
#define ASI_REGMAP          0x06
#define ASI_HWFLUSHCONTEXT  0x07

#define ASI_USERTXT         0x08
#define ASI_KERNELTXT       0x09
#define ASI_USERDATA        0x0a
#define ASI_KERNELDATA      0x0b

/* VAC Cache flushing on sun4c and sun4 */
#define ASI_FLUSHSEG        0x0c
#define ASI_FLUSHPG         0x0d
#define ASI_FLUSHCTX        0x0e

/* SPARCstation-5: only 6 bits are decoded. */
/* wo = Write Only, rw = Read Write;        */
/* ss = Single Size, as = All Sizes;        */
#define ASI_M_RES00         0x00   /* Don't touch... */
#define ASI_M_UNA01         0x01   /* Same here... */
#define ASI_M_MXCC          0x02   /* Access to TI VIKING MXCC registers */
#define ASI_M_FLUSH_PROBE   0x03   /* Reference MMU Flush/Probe; rw, ss */
#define ASI_M_MMUREGS       0x04   /* MMU Registers; rw, ss */
#define ASI_M_TLBDIAG       0x05   /* MMU TLB only Diagnostics */
#define ASI_M_DIAGS         0x06   /* Reference MMU Diagnostics */
#define ASI_M_IODIAG        0x07   /* MMU I/O TLB only Diagnostics */
#define ASI_M_USERTXT       0x08   /* Same as ASI_USERTXT; rw, as */
#define ASI_M_KERNELTXT     0x09   /* Same as ASI_KERNELTXT; rw, as */
#define ASI_M_USERDATA      0x0A   /* Same as ASI_USERDATA; rw, as */
#define ASI_M_KERNELDATA    0x0B   /* Same as ASI_KERNELDATA; rw, as */
#define ASI_M_TXTC_TAG      0x0C   /* Instruction Cache Tag; rw, ss */
#define ASI_M_TXTC_DATA     0x0D   /* Instruction Cache Data; rw, ss */
#define ASI_M_DATAC_TAG     0x0E   /* Data Cache Tag; rw, ss */
#define ASI_M_DATAC_DATA    0x0F   /* Data Cache Data; rw, ss */

/* The following cache flushing ASIs work only with the 'sta'
 * instruction. Results are unpredictable for 'swap' and 'ldstuba',
 * so don't do it.
 */

/* These ASI flushes affect external caches too. */
#define ASI_M_FLUSH_PAGE    0x10   /* Flush I&D Cache Line (page); wo, ss */
#define ASI_M_FLUSH_SEG     0x11   /* Flush I&D Cache Line (seg); wo, ss */
#define ASI_M_FLUSH_REGION  0x12   /* Flush I&D Cache Line (region); wo, ss */
#define ASI_M_FLUSH_CTX     0x13   /* Flush I&D Cache Line (context); wo, ss */
#define ASI_M_FLUSH_USER    0x14   /* Flush I&D Cache Line (user); wo, ss */

/* Block-copy operations are available only on certain V8 cpus. */
#define ASI_M_BCOPY         0x17   /* Block copy */

/* These affect only the ICACHE and are Ross HyperSparc and TurboSparc specific. */
#define ASI_M_IFLUSH_PAGE   0x18   /* Flush I Cache Line (page); wo, ss */
#define ASI_M_IFLUSH_SEG    0x19   /* Flush I Cache Line (seg); wo, ss */
#define ASI_M_IFLUSH_REGION 0x1A   /* Flush I Cache Line (region); wo, ss */
#define ASI_M_IFLUSH_CTX    0x1B   /* Flush I Cache Line (context); wo, ss */
#define ASI_M_IFLUSH_USER   0x1C   /* Flush I Cache Line (user); wo, ss */

/* Block-fill operations are available on certain V8 cpus */
#define ASI_M_BFILL         0x1F

/* This allows direct access to main memory, actually 0x20 to 0x2f are
 * the available ASI's for physical ram pass-through, but I don't have
 * any idea what the other ones do....
 */

#define ASI_M_BYPASS       0x20   /* Reference MMU bypass; rw, as */
#define ASI_M_FBMEM        0x29   /* Graphics card frame buffer access */
#define ASI_M_VMEUS        0x2A   /* VME user 16-bit access */
#define ASI_M_VMEPS        0x2B   /* VME priv 16-bit access */
#define ASI_M_VMEUT        0x2C   /* VME user 32-bit access */
#define ASI_M_VMEPT        0x2D   /* VME priv 32-bit access */
#define ASI_M_SBUS         0x2E   /* Direct SBus access */
#define ASI_M_CTL          0x2F   /* Control Space (ECC and MXCC are here) */


/* This is ROSS HyperSparc only. */
#define ASI_M_FLUSH_IWHOLE 0x31   /* Flush entire ICACHE; wo, ss */

/* Tsunami/Viking/TurboSparc i/d cache flash clear. */
#define ASI_M_IC_FLCLEAR   0x36
#define ASI_M_DC_FLCLEAR   0x37

#define ASI_M_DCDR         0x39   /* Data Cache Diagnostics Register rw, ss */

#define ASI_M_VIKING_TMP1  0x40	  /* Emulation temporary 1 on Viking */
/* only available on SuperSparc I */
/* #define ASI_M_VIKING_TMP2  0x41 */  /* Emulation temporary 2 on Viking */

#define ASI_M_ACTION       0x4c   /* Breakpoint Action Register (GNU/Viking) */

/* FIXME: non-standard Leon ASI definition */
#if defined(CONFIG_LEON) || defined(CONFIG_LEON3)
#undef ASI_M_MMUREGS
#define ASI_M_MMUREGS		0x19
#endif /* CONFIG_LEON */

#ifndef CONFIG_LEON_3

/* ASI codes */
#define ASI_LEON_PCI		0x04
#define ASI_LEON_IFLUSH		0x05
#define ASI_LEON_DFLUSH		0x06
#define ASI_LEON_ITAG		0x0c
#define ASI_LEON_IDATA		0x0d
#define ASI_LEON_DTAG		0x0e
#define ASI_LEON_DDATA		0x0f
#define ASI_LEON_MMUFLUSH	0x18
#define ASI_LEON_MMUREGS	0x19
#define ASI_LEON_BYPASS		0x1c
#define ASI_LEON_FLUSH_PAGE	0x10
/*
#define ASI_LEON_FLUSH_SEGMENT	0x11
#define ASI_LEON_FLUSH_REGION	0x12
*/
#define ASI_LEON_FLUSH_CTX	0x13
#define ASI_LEON_DCTX		0x14
#define ASI_LEON_ICTX		0x15
#define ASI_MMU_DIAG		0x1d

#define ASI_LEON_DCACHE_MISS	0x1

#else

#define ASI_LEON_DCACHE_MISS	0x1

#define ASI_LEON_CACHEREGS 0x02
#define ASI_LEON_IFLUSH		0x10
#define ASI_LEON_DFLUSH		0x11

#define ASI_LEON_MMUFLUSH	0x18
#define ASI_LEON_MMUREGS	0x19
#define ASI_LEON_BYPASS		0x1c
#define ASI_LEON_FLUSH_PAGE	0x10

/*
constant ASI_SYSR    : asi_type := "00010"; -- 0x02
constant ASI_UINST   : asi_type := "01000"; -- 0x08
constant ASI_SINST   : asi_type := "01001"; -- 0x09
constant ASI_UDATA   : asi_type := "01010"; -- 0x0A
constant ASI_SDATA   : asi_type := "01011"; -- 0x0B
constant ASI_ITAG    : asi_type := "01100"; -- 0x0C
constant ASI_IDATA   : asi_type := "01101"; -- 0x0D
constant ASI_DTAG    : asi_type := "01110"; -- 0x0E
constant ASI_DDATA   : asi_type := "01111"; -- 0x0F
constant ASI_IFLUSH  : asi_type := "10000"; -- 0x10
constant ASI_DFLUSH  : asi_type := "10001"; -- 0x11

constant ASI_FLUSH_PAGE     : std_logic_vector(4 downto 0) := "10000";  -- 0x10 i/dcache flush page
constant ASI_FLUSH_CTX      : std_logic_vector(4 downto 0) := "10011";  -- 0x13 i/dcache flush ctx

constant ASI_DCTX           : std_logic_vector(4 downto 0) := "10100";  -- 0x14 dcache ctx
constant ASI_ICTX           : std_logic_vector(4 downto 0) := "10101";  -- 0x15 icache ctx

constant ASI_MMUFLUSHPROBE  : std_logic_vector(4 downto 0) := "11000";  -- 0x18 i/dtlb flush/(probe)
constant ASI_MMUREGS        : std_logic_vector(4 downto 0) := "11001";  -- 0x19 mmu regs access
constant ASI_MMU_BP         : std_logic_vector(4 downto 0) := "11100";  -- 0x1c mmu Bypass
constant ASI_MMU_DIAG       : std_logic_vector(4 downto 0) := "11101";  -- 0x1d mmu diagnostic
constant ASI_MMU_DSU        : std_logic_vector(4 downto 0) := "11111";  -- 0x1f mmu diagnostic
*/
#endif

#endif /* SPARC_ASI_H */

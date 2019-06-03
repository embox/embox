/**
 * @file PowerPC 440 definitions
 *
 * @date 17.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef PPC_REGS_H_
#define PPC_REGS_H_

/**
 * Machine State Register (MSR) definations
 */
#define MSR_WE  0x00040000 /* wait state enable */
#define MSR_CE  0x00020000 /* critical interrupt enable */
#define	MSR_ILE 0x00010000 /* exception little-endian mode (1:LE) */
#define MSR_EE  0x00008000 /* external interrupt enable */
#define	MSR_PR  0x00004000 /* privilege level (1:USR) */
#define MSR_FP  0x00002000 /* floating-point available */
#define MSR_ME  0x00001000 /* machine check enable */
#define MSR_FE0 0x00000800 /* floating-point exception mode 0 */
#define MSR_DWE 0x00000400 /* debug wait enable */
#define MSR_DE  0x00000200 /* debug interrupt enable */
#define MSR_FE1 0x00000100 /* floating-point exception mode 1 */
#define	MSR_IP  0x00000040 /* exception prefix */
#define MSR_IS  0x00000020 /* instruction address space */
#define MSR_DS  0x00000010 /* data address space */
#define	MSR_RI  0x00000002 /* recoverable exception */
#define	MSR_LE  0x00000001 /* little-endian mode enable */

/**
 * Timer Control Register (TCR) definations
 */
    /* Watchdog Timer Period */
#define TCR_WP_21    0x00000000 /* WTP: 2^21 time base clocks */
#define TCR_WP_25    0x40000000 /* WTP: 2^25 time base clocks */
#define TCR_WP_29    0x80000000 /* WTP: 2^29 time base clocks */
#define TCR_WP_33    0xC0000000 /* WTP: 2^33 time base clocks */
    /* Watchdog Timer Reset Control */
#define TCR_WRC_NO   0x00000000 /* WTRC: no watchdog timer reset will occur */
#define TCR_WRC_CORE 0x10000000 /* WTRC: core reset */
#define TCR_WRC_CHIP 0x20000000 /* WTRC: chip reset */
#define TCR_WRC_SYS  0x30000000 /* WTRC: system reset */
    /* Watchdog Timer Interrupt */
#define TCR_WIE      0x08000000 /* WTI enable */
    /* Decrementer Interrupt */
#define TCR_DIE      0x04000000 /* DI enable */
    /* Fixed Interval Timer Period */
#define TCR_FP_13    0x00000000 /* FITP: 2^13 time base clocks */
#define TCR_FP_17    0x01000000 /* FITP: 2^17 time base clocks */
#define TCR_FP_21    0x02000000 /* FITP: 2^21 time base clocks */
#define TCR_FP_25    0x03000000 /* FITP: 2^25 time base clocks */
    /* Fixed Interval Timer Interrupt */
#define TCR_FIE      0x00800000 /* FITI enable */
    /* Auto-Reload */
#define TCR_ARE      0x00400000 /* AR enable */

/**
 * Timer Status Register (TSR) definations
 */
    /* Next Watchdog Timer Exception */
#define TSR_ENW      0x00000001 /* action on next NWTE is governed by TSR[WIS] */
    /* Watchdog Timer Interrupt Status */
#define TSR_WIS      0x00000002 /* WTI has occured */
    /* Watchdog Timer Reset Status */
#define TSR_WRS_NO   0x00000000 /* WTRS: no watchdog reset has occured */
#define TSR_WRS_CORE 0x00000004 /* WTRS: core reset was forced by watchdog timer */
#define TSR_WRS_CHIP 0x00000008 /* WTRS: chip reset was forced by watchdog timer */
#define TSR_WRS_SYS  0x0000000C /* WTRS: system reset was forced by watchdog timer */
    /* Decrementer Interrupt Status */
#define TSR_DIS      0x00000010 /* DI has occured */
    /* Fixed Interval Timer Interrupt Status */
#define TSR_FIS      0x00000020 /* FITI has occured */

#if 0
/**
 * Special Purpose Register (SPR) declarations
 */
#define SPR_XER     0x001 /* integer exception register (U:RW) */
#define SPR_LR      0x008 /* link register (U:RW) */
#define SPR_CTR     0x009 /* count register (U:RW) */
#define SPR_DEC     0x016 /* decrementer (S:RW) */
#define SPR_SRR0    0x01A /* save/restore register 0 (S:RW) */
#define SPR_SRR1    0x01B /* save/restore register 1 (S:RW) */
#define SPR_PID     0x030 /* process ID (S:RW) */
#define SPR_DECAR   0x036 /* decrementer auto-reload (S:WO) */
#define SPR_CSRR0   0x03A /* critical save/restore register 0 (S:RW) */
#define SPR_CSRR1   0x03B /* critical save/restore register 1 (S:RW) */
#define SPR_DEAR    0x03D /* data exception address register (S:RW) */
#define SPR_ESR     0x03E /* exception syndrome register (S:RW) */
#define SPR_IVPR    0x03F /* interrupt vector prefix register (S:RW) */
#define SPR_USPRG0  0x100 /* user special purpose register general 0 (U:RW) */
#define SPR_SPRG4_R 0x104 /* special purpose register general 4 (U:RO) */
#define SPR_SPRG5_R 0x105 /* special purpose register general 5 (U:RO) */
#define SPR_SPRG6_R 0x106 /* special purpose register general 6 (U:RO) */
#define SPR_SPRG7_R 0x107 /* special purpose register general 7 (U:RO) */
#define SPR_TBL_R   0x10C /* time base lower (U:RO) */
#define SPR_TBU_R   0x10D /* time base upper (U:RO) */
#define SPR_SPRG0   0x110 /* special purpose register general 0 (S:RW) */
#define SPR_SPRG1   0x111 /* special purpose register general 1 (S:RW) */
#define SPR_SPRG2   0x112 /* special purpose register general 2 (S:RW) */
#define SPR_SPRG3   0x113 /* special purpose register general 3 (S:RW) */
#define SPR_SPRG4_W 0x114 /* special purpose register general 4 (S:WO) */
#define SPR_SPRG5_W 0x115 /* special purpose register general 5 (S:WO) */
#define SPR_SPRG6_W 0x116 /* special purpose register general 6 (S:WO) */
#define SPR_SPRG7_W 0x117 /* special purpose register general 7 (S:WO) */
#define SPR_TBL_W   0x11C /* time base lower (S:WO) */
#define SPR_TBU_W   0x11D /* time base upper (S:WO) */
#define SPR_PIR     0x11E /* processor ID register (S:RO) */
#define SPR_PVR     0x11F /* processor version register (S:RO) */
#define SPR_DBSR    0x130 /* debug status register (S:RC) */
#define SPR_DBCR0   0x134 /* debug control register 0 (S:RW) */
#define SPR_DBCR1   0x135 /* debug control register 1 (S:RW) */
#define SPR_DBCR2   0x136 /* debug control register 2 (S:RW) */
#define SPR_IAC1    0x138 /* instruction address compare 1 (S:RW) */
#define SPR_IAC2    0x139 /* instruction address compare 2 (S:RW) */
#define SPR_IAC3    0x13A /* instruction address compare 3 (S:RW) */
#define SPR_IAC4    0x13B /* instruction address compare 4 (S:RW) */
#define SPR_DAC1    0x13C /* data address compare 1 (S:RW) */
#define SPR_DAC2    0x13D /* data address compare 2 (S:RW) */
#define SPR_DVC1    0x13E /* data value compare 1 (S:RW) */
#define SPR_DVC2    0x13F /* data value compare 2 (S:RW) */
#define SPR_TSR     0x150 /* timer status register (S:RC) */
#define SPR_TCR     0x154 /* timer control register (S:RW) */
#define SPR_IVOR0   0x190 /* interrupt vector offset register 0 (S:RW) */
#define SPR_IVOR1   0x191 /* interrupt vector offset register 1 (S:RW) */
#define SPR_IVOR2   0x192 /* interrupt vector offset register 2 (S:RW) */
#define SPR_IVOR3   0x193 /* interrupt vector offset register 3 (S:RW) */
#define SPR_IVOR4   0x194 /* interrupt vector offset register 4 (S:RW) */
#define SPR_IVOR5   0x195 /* interrupt vector offset register 5 (S:RW) */
#define SPR_IVOR6   0x196 /* interrupt vector offset register 6 (S:RW) */
#define SPR_IVOR7   0x197 /* interrupt vector offset register 7 (S:RW) */
#define SPR_IVOR8   0x198 /* interrupt vector offset register 8 (S:RW) */
#define SPR_IVOR9   0x199 /* interrupt vector offset register 9 (S:RW) */
#define SPR_IVOR10  0x19A /* interrupt vector offset register 10 (S:RW) */
#define SPR_IVOR11  0x19B /* interrupt vector offset register 11 (S:RW) */
#define SPR_IVOR12  0x19C /* interrupt vector offset register 12 (S:RW) */
#define SPR_IVOR13  0x19D /* interrupt vector offset register 13 (S:RW) */
#define SPR_IVOR14  0x19E /* interrupt vector offset register 14 (S:RW) */
#define SPR_IVOR15  0x19F /* interrupt vector offset register 15 (S:RW) */
#define SPR_MCSRR0  0x23A /* machine check save/resotre register 0 (S:RW) */
#define SPR_MCSRR1  0x23B /* machine check save/resotre register 1 (S:RW) */
#define SPR_MCSR    0x23C /* machine check status register (S:RW) */
#define SPR_INV0    0x370 /* instruction cache normal victim 0 (S:RW) */
#define SPR_INV1    0x371 /* instruction cache normal victim 1 (S:RW) */
#define SPR_INV2    0x372 /* instruction cache normal victim 2 (S:RW) */
#define SPR_INV3    0x373 /* instruction cache normal victim 3 (S:RW) */
#define SPR_ITV0    0x374 /* instruction cache transient victim 0 (S:RW) */
#define SPR_ITV1    0x375 /* instruction cache transient victim 1 (S:RW) */
#define SPR_ITV2    0x376 /* instruction cache transient victim 2 (S:RW) */
#define SPR_ITV3    0x377 /* instruction cache transient victim 3 (S:RW) */
#define SPR_CCR1    0x378 /* core configuration register 1 (S:RW) */
#define SPR_DNV0    0x390 /* data cache normal victim 0 (S:RW) */
#define SPR_DNV1    0x391 /* data cache normal victim 1 (S:RW) */
#define SPR_DNV2    0x392 /* data cache normal victim 2 (S:RW) */
#define SPR_DNV3    0x393 /* data cache normal victim 3 (S:RW) */
#define SPR_DTV0    0x394 /* data cache transient victim 0 (S:RW) */
#define SPR_DTV1    0x395 /* data cache transient victim 1 (S:RW) */
#define SPR_DTV2    0x396 /* data cache transient victim 2 (S:RW) */
#define SPR_DTV3    0x397 /* data cache transient victim 3 (S:RW) */
#define SPR_DVLIM   0x398 /* data cache victim limit (S:RW) */
#define SPR_IVLIM   0x399 /* instruction cache victim limit (S:RW) */
#define SPR_RSTCFG  0x39B /* reset configuration (S:RO) */
#define SPR_DCDBTRL 0x39C /* data cache debug tag register low (S:RO) */
#define SPR_DCDBTRH 0x39D /* data cache debug tag register high (S:RO) */
#define SPR_ICDBTRL 0x39E /* instruction cache debug tag register low (S:RO) */
#define SPR_ICDBTRH 0x39F /* instruction cache debug tag register high (S:RO) */
#define SPR_MMUCR   0x3B2 /* memory management unit control register (S:RW) */
#define SPR_CCR0    0x3B3 /* core configuration register 0 (S:RW) */
#define SPR_ICDBDR  0x3D3 /* instruction cache debug data register (S:RO) */
#define SPR_DBDR    0x3F3 /* debug data register (S:RW) */
#endif

#endif /* PPC_REGS_H_ */

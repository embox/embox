/**
 * @file
 *
 * @date Mar 23, 2020
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_INTERRUPT_MIPS_GIC_MIPS_GIC_REGS_H_
#define SRC_DRIVERS_INTERRUPT_MIPS_GIC_MIPS_GIC_REGS_H_

#define MIPS_GIC_INTERRUPT_PIN   3

#define GIC_SH_CONFIG            0x0000
#define GIC_SH_COUNTER_LO        0x0010
#define GIC_SH_COUNTER_HI        0x0014
#define GIC_SH_REVID             0x0020

#define GIC_SH_INT_AVAIL(N)      (0x0024 + (((N) >> 5) << 2))
#define GIC_SH_INT_AVAIL31_0     0x0024
#define GIC_SH_INT_AVAIL63_32    0x0028
#define GIC_SH_INT_AVAIL95_64    0x002C
#define GIC_SH_INT_AVAIL127_96   0x0030
#define GIC_SH_INT_AVAIL159_128  0x0034
#define GIC_SH_INT_AVAIL191_160  0x0038
#define GIC_SH_INT_AVAIL223_192  0x003C
#define GIC_SH_INT_AVAIL255_224  0x0040

#define GIC_SH_GID_CONFIG(N)     (0x0080 + (((N) >> 5) << 2))
#define GIC_SH_GID_CONFIG31_0    0x0080
#define GIC_SH_GID_CONFIG63_32   0x0084
#define GIC_SH_GID_CONFIG95_64   0x0088
#define GIC_SH_GID_CONFIG127_96  0x008C
#define GIC_SH_GID_CONFIG159_128 0x0090
#define GIC_SH_GID_CONFIG191_160 0x0094
#define GIC_SH_GID_CONFIG223_192 0x0098
#define GIC_SH_GID_CONFIG255_224 0x009C

#define GIC_SH_POL(N)            (0x0100 + (((N) >> 5) << 2))
#define GIC_SH_POL31_0           0x0100
#define GIC_SH_POL63_32          0x0104
#define GIC_SH_POL95_64          0x0108
#define GIC_SH_POL127_96         0x010C
#define GIC_SH_POL159_128        0x0110
#define GIC_SH_POL191_160        0x0114
#define GIC_SH_POL223_192        0x0118
#define GIC_SH_POL255_224        0x011C

#define GIC_SH_TRIG(N)           (0x0180 + (((N) >> 5) << 2))
#define GIC_SH_TRIG31_0          0x0180
#define GIC_SH_TRIG63_32         0x0184
#define GIC_SH_TRIG95_64         0x0188
#define GIC_SH_TRIG127_96        0x018C
#define GIC_SH_TRIG159_128       0x0190
#define GIC_SH_TRIG191_160       0x0194
#define GIC_SH_TRIG223_192       0x0198
#define GIC_SH_TRIG255_224       0x019C

#define GIC_SH_DUAL(N)           (0x0200 + (((N) >> 5) << 2))
#define GIC_SH_DUAL31_0          0x0200

#define GIC_SH_WEDGE(N)          (0x0280 + (((N) >> 5) << 2))

#define GIC_SH_RMASK(N)          (0x0300 + (((N) >> 5) << 2))
#define GIC_SH_RMASK31_0         0x0300

#define GIC_SH_SMASK(N)          (0x0380 + (((N) >> 5) << 2))
#define GIC_SH_SMASK31_0         0x0380

#define GIC_SH_MASK(N)           (0x0400 + (((N) >> 5) << 2))
#define GIC_SH_MASK31_0          0x0400

#define GIC_SH_PEND(N)           (0x0480 + (((N) >> 5) << 2))
#define GIC_SH_PEND31_0          0x0480
#define GIC_SH_PEND63_32         0x0484
#define GIC_SH_PEND95_64         0x0488
#define GIC_SH_PEND127_96        0x048C
#define GIC_SH_PEND159_128       0x0490
#define GIC_SH_PEND191_160       0x0494
#define GIC_SH_PEND223_192       0x0498
#define GIC_SH_PEND255_224       0x049C

#define GIC_SH_MAP_PIN(N)        (0x0500 + ((N) << 2))
#define GIC_SH_MAP0_PIN          0x0500

#define GIC_SH_MAP_CORE31_0(N)   (0x2000 + (((N) << 5)))
#define GIC_SH_MAP0_CORE31_0     0x2000

#define GIC_VB_DINT_SEND         0x6000

#endif /* SRC_DRIVERS_INTERRUPT_MIPS_GIC_MIPS_GIC_REGS_H_ */

/**
 * @file
 *
 * @date 28.03.2017
 * @author Anton Bondarev
 */

#ifndef PL310_H_
#define PL310_H_

#define BIT(n)         (1 << n)

#define L2X0_CACHE_ID                    (PL310_BASE + 0x000)
#define L2X0_CACHE_TYPE                  (PL310_BASE + 0x004)
#define L2X0_CTRL                        (PL310_BASE + 0x100)
#define L2X0_AUX_CTRL                    (PL310_BASE + 0x104)
# define L2X0_AUX_INSTR_PREFETCH         (1 << 29)
# define L2X0_AUX_DATA_PREFETCH          (1 << 28)
#define L310_TAG_LATENCY_CTRL            (PL310_BASE + 0x108)
#define L310_DATA_LATENCY_CTRL           (PL310_BASE + 0x10C)
#define L2X0_EVENT_CNT_CTRL              (PL310_BASE + 0x200)
#define L2X0_EVENT_CNT1_CFG              (PL310_BASE + 0x204)
#define L2X0_EVENT_CNT0_CFG              (PL310_BASE + 0x208)
#define L2X0_EVENT_CNT1_VAL              (PL310_BASE + 0x20C)
#define L2X0_EVENT_CNT0_VAL              (PL310_BASE + 0x210)
#define L2X0_INTR_MASK                   (PL310_BASE + 0x214)
#define L2X0_MASKED_INTR_STAT            (PL310_BASE + 0x218)
#define L2X0_RAW_INTR_STAT               (PL310_BASE + 0x21C)
#define L2X0_INTR_CLEAR                  (PL310_BASE + 0x220)
#define L2X0_CACHE_SYNC                  (PL310_BASE + 0x730)
#define L2X0_DUMMY_REG                   (PL310_BASE + 0x740)
#define L2X0_INV_LINE_PA                 (PL310_BASE + 0x770)
#define L2X0_INV_WAY                     (PL310_BASE + 0x77C)
#define L2X0_CLEAN_LINE_PA               (PL310_BASE + 0x7B0)
#define L2X0_CLEAN_LINE_IDX              (PL310_BASE + 0x7B8)
#define L2X0_CLEAN_WAY                   (PL310_BASE + 0x7BC)
#define L2X0_CLEAN_INV_LINE_PA           (PL310_BASE + 0x7F0)
#define L2X0_CLEAN_INV_LINE_IDX          (PL310_BASE + 0x7F8)
#define L2X0_CLEAN_INV_WAY               (PL310_BASE + 0x7FC)
#define L2X0_LOCKDOWN_WAY_D_BASE         (PL310_BASE + 0x900)
#define L2X0_LOCKDOWN_WAY_I_BASE         (PL310_BASE + 0x904)
#define L2X0_LOCKDOWN_STRIDE             (PL310_BASE + 0x908)
#define L310_ADDR_FILTER_START           (PL310_BASE + 0xC00)
#define L310_ADDR_FILTER_END             (PL310_BASE + 0xC04)
#define L2X0_TEST_OPERATION              (PL310_BASE + 0xF00)
#define L2X0_LINE_DATA                   (PL310_BASE + 0xF10)
#define L2X0_LINE_TAG                    (PL310_BASE + 0xF30)
#define L2X0_DEBUG_CTRL                  (PL310_BASE + 0xF40)
#define L310_PREFETCH_CTRL               (PL310_BASE + 0xF60)
#define L310_POWER_CTRL                  (PL310_BASE + 0xF80)
#define L70_INVAL	                 (PL310_BASE + 0xFFC)
# define   L310_DYNAMIC_CLK_GATING_EN    (1 << 1)
# define   L310_STNDBY_MODE_EN           (1 << 0)

/* Registers shifts and masks */
# define L2X0_CACHE_ID_PART_MASK         (0xf << 6)
# define L2X0_CACHE_ID_PART_L210         (1 << 6)
# define L2X0_CACHE_ID_PART_L220         (2 << 6)
# define L2X0_CACHE_ID_PART_L310         (3 << 6)
# define L2X0_CACHE_ID_RTL_MASK          0x3f
# define L210_CACHE_ID_RTL_R0P2_02       0x00
# define L210_CACHE_ID_RTL_R0P1          0x01
# define L210_CACHE_ID_RTL_R0P2_01       0x02
# define L210_CACHE_ID_RTL_R0P3          0x03
# define L210_CACHE_ID_RTL_R0P4          0x0b
# define L210_CACHE_ID_RTL_R0P5          0x0f
# define L220_CACHE_ID_RTL_R1P7_01REL0   0x06
# define L310_CACHE_ID_RTL_R0P0          0x00
# define L310_CACHE_ID_RTL_R1P0          0x02
# define L310_CACHE_ID_RTL_R2P0          0x04
# define L310_CACHE_ID_RTL_R3P0          0x05
# define L310_CACHE_ID_RTL_R3P1          0x06
# define L310_CACHE_ID_RTL_R3P1_50REL0   0x07
# define L310_CACHE_ID_RTL_R3P2          0x08
# define L310_CACHE_ID_RTL_R3P3          0x09

/* L2C auxiliary control register - bits common to L2C-210/220/310 */
#define L2C_AUX_CTRL_WAY_SIZE_SHIFT        17
#define L2C_AUX_CTRL_WAY_SIZE_MASK         (7 << 17)
#define L2C_AUX_CTRL_WAY_SIZE(n)           ((n) << 17)
#define L2C_AUX_CTRL_EVTMON_ENABLE         BIT(20)
#define L2C_AUX_CTRL_PARITY_ENABLE         BIT(21)
#define L2C_AUX_CTRL_SHARED_OVERRIDE       BIT(22)
 /* L2C-310 specific bits */
#define L310_AUX_CTRL_FULL_LINE_ZERO       BIT(0)
#define L310_AUX_CTRL_HIGHPRIO_SO_DEV      BIT(10)
#define L310_AUX_CTRL_STORE_LIMITATION     BIT(11)
#define L310_AUX_CTRL_EXCLUSIVE_CACHE      BIT(12)
#define L310_AUX_CTRL_ASSOCIATIVITY_16     BIT(16)
#define L310_AUX_CTRL_CACHE_REPLACE_RR     BIT(25)
#define L310_AUX_CTRL_NS_LOCKDOWN          BIT(26)
#define L310_AUX_CTRL_NS_INT_CTRL          BIT(27)
#define L310_AUX_CTRL_DATA_PREFETCH        BIT(28)
#define L310_AUX_CTRL_INSTR_PREFETCH       BIT(29)
#define L310_AUX_CTRL_EARLY_BRESP          BIT(30)

#define L310_LATENCY_CTRL_SETUP(n)         ((n) << 0)
#define L310_LATENCY_CTRL_RD(n)            ((n) << 4)
#define L310_LATENCY_CTRL_WR(n)            ((n) << 8)
#define L310_ADDR_FILTER_EN                1

#define L310_PREFETCH_CTRL_OFFSET_MASK        0x1f
#define L310_PREFETCH_CTRL_DBL_LINEFILL_INCR  BIT(23)
#define L310_PREFETCH_CTRL_PREFETCH_DROP      BIT(24)
#define L310_PREFETCH_CTRL_DBL_LINEFILL_WRAP  BIT(27)
#define L310_PREFETCH_CTRL_DATA_PREFETCH      BIT(28)
#define L310_PREFETCH_CTRL_INSTR_PREFETCH     BIT(29)
#define L310_PREFETCH_CTRL_DBL_LINEFILL       BIT(30)
#define L2X0_CTRL_EN                    1
#define L2X0_WAY_SIZE_SHIFT             3

#endif /* PL310_H_ */

/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#ifndef DRIVERS_DMA_NIIET_DMA_REGS_H_
#define DRIVERS_DMA_NIIET_DMA_REGS_H_

#include <stdint.h>

#define NIIET_DMA_CH_MAX 32

struct niiet_dma_ch_regs {
	volatile uint32_t SRC_PTR;
	volatile uint32_t DST_PTR;

	/*!< Command register 2 (number of data to transfer, linear mode) */
	volatile uint32_t NDTL;

	/*!< Command register 3 (config) */
	volatile uint32_t CONFIG;

	/*!< Static register 0 */
	volatile uint32_t STATIC0;

	/*!< Static register 1 */
	volatile uint32_t STATIC1;

	/*!< Static register 2 */
	volatile uint32_t STATIC2;

	/*!< Static register 3 */
	volatile uint32_t STATIC3;

	/*!< Static register 4 */
	volatile uint32_t STATIC4;

	/*!< Channels restrictions status register */
	volatile uint32_t RESTRICT;

	/*!< Channels read offset status register */
	volatile uint32_t RDOFFSET;

	/*!< Channels write offset status register */
	volatile uint32_t WROFFSET;

	/*!< FIFO fullness status register */
	volatile uint32_t FIFOSTAT;

	/*!< Outstanding commands status register */
	volatile uint32_t CMD_OUTS;

	volatile uint32_t Reserved0[2];

	/*!< Channel active register */
	volatile uint32_t CH_ACTIVE;

	/*!< Channel start register */
	volatile uint32_t CH_START;

	/*!< Channel status register */
	volatile uint32_t CH_STATUS;

	/*!< Buffer counter status register */
	volatile uint32_t COUNT;

	/*!< Interrupt raw status register */
	volatile uint32_t INT_RAWSTAT;

	/*!< Interrupt clear register */
	volatile uint32_t INT_CLEAR;

	/*!< Interrupt enable register */
	volatile uint32_t INT_ENABLE;

	/*!< Interrupt status */
	volatile uint32_t INT_STATUS;

	/*!< Channel error stop condition register */
	volatile uint32_t ERR_STOP;

	volatile uint32_t ERR_RD_ADDR;
	volatile uint32_t ERR_WR_ADDR; /* 0x68 */
	volatile uint32_t Reserved1[5];
};

#define DMA_CH_STATIC0_RD_BURST_MAX_Pos 0
#define DMA_CH_STATIC0_RD_TOKENS_Pos    16
#define DMA_CH_STATIC0_RD_OUTS_MAX_Pos  24
#define DMA_CH_STATIC0_RD_OUTSTCFG_Pos  30
#define DMA_CH_STATIC0_RD_INCR_Pos      31

#define DMA_CH_STATIC0_RD_BURST_MAX(v) \
	((v & 0x7F) << DMA_CH_STATIC0_RD_BURST_MAX_Pos)
#define DMA_CH_STATIC0_RD_TOKENS(v)    \
	((v & 0x3F) << DMA_CH_STATIC0_RD_TOKENS_Pos)
#define DMA_CH_STATIC0_RD_OUTS_MAX(v)  \
	((v & 0x0F) << DMA_CH_STATIC0_RD_OUTS_MAX_Pos)
#define DMA_CH_STATIC0_RD_OUTSTCFG(v)  \
	((v & 0x1) << DMA_CH_STATIC0_RD_OUTSTCFG_Pos)
#define DMA_CH_STATIC0_RD_INCR(v)      \
	((v & 0x1) << DMA_CH_STATIC0_RD_INCR_Pos)

#define DMA_CH_STATIC1_WR_BURST_MAX_Pos 0
#define DMA_CH_STATIC1_WR_TOKENS_Pos    16
#define DMA_CH_STATIC1_WR_OUTS_MAX_Pos  24
#define DMA_CH_STATIC1_WR_OUTSTCFG_Pos  30
#define DMA_CH_STATIC1_WR_INCR_Pos      31

#define DMA_CH_STATIC1_WR_BURST_MAX(v) \
	((v & 0x7F) << DMA_CH_STATIC1_WR_BURST_MAX_Pos)
#define DMA_CH_STATIC1_WR_TOKENS(v)    \
	((v & 0x3F) << DMA_CH_STATIC1_WR_TOKENS_Pos)
#define DMA_CH_STATIC1_WR_OUTS_MAX(v)) \
	((v & 0x0F) <<  DMA_CH_STATIC1_WR_OUTS_MAX_Pos )
#define DMA_CH_STATIC1_WR_OUTSTCFG(v) \
	((v & 0x1) << DMA_CH_STATIC1_WR_OUTSTCFG_Pos)
#define DMA_CH_STATIC1_WR_INCR(v)    \
	((v & 0x1) << DMA_CH_STATIC1_WR_INCR_Pos)

#define DMA_CH_STATIC2_FRAME_WIDTH_Pos 0
#define DMA_CH_STATIC2_BLOCK_Pos       15
#define DMA_CH_STATIC2_JOINT_Pos       16
#define DMA_CH_STATIC2_RETRY_Pos       17
#define DMA_CH_STATIC2_END_SWAP_Pos    28

#define DMA_CH_STATIC2_FRAME_WIDTH(v) \
	((v & 0xFFF) << DMA_CH_STATIC2_FRAME_WIDTH_Pos)
#define DMA_CH_STATIC2_BLOCK(v)       \
	((v & 0x01) << DMA_CH_STATIC2_BLOCK_Pos)
#define DMA_CH_STATIC2_JOINT(v)       \
	((v & 0x01) << DMA_CH_STATIC2_JOINT_Pos)
#define DMA_CH_STATIC2_RETRY(v)      \
	((v & 0x01) << DMA_CH_STATIC2_RETRY_Pos)
#define DMA_CH_STATIC2_END_SWAP(v)   \
	((v & 0x03) << DMA_CH_STATIC2_END_SWAP_Pos)

#define DMA_CH_STATIC3_RD_WAIT_LIM_Pos 0
#define DMA_CH_STATIC3_WR_WAIT_LIM_Pos 16

#define DMA_CH_STATIC3_RD_WAIT_LIM(v) \
	((v & 0xFFF) << DMA_CH_STATIC3_RD_WAIT_LIM_Pos)
#define DMA_CH_STATIC3_WR_WAIT_LIM(v) \
	((v & 0xFFF) << DMA_CH_STATIC3_WR_WAIT_LIM_Pos)

#define DMA_CH_STATIC4_RD_PER_NUM_Pos   0
#define DMA_CH_STATIC4_RD_PER_DELAY_Pos 8
#define DMA_CH_STATIC4_RD_PER_BLOCK_Pos 15
#define DMA_CH_STATIC4_WR_PER_NUM_Pos   16
#define DMA_CH_STATIC4_WR_PER_DELAY_Pos 24
#define DMA_CH_STATIC4_WR_PER_BLOCK_Pos 31

#define DMA_CH_STATIC4_RD_PER_NUM(v) \
	((v & 0x7F) << DMA_CH_STATIC4_RD_PER_NUM_Pos)
#define DMA_CH_STATIC4_RD_PER_DELAY(v) \
	((v & 0x07) << DMA_CH_STATIC4_RD_PER_DELAY_Pos)
#define DMA_CH_STATIC4_RD_PER_BLOCK(v) \
	((v & 0x01) << DMA_CH_STATIC4_RD_PER_BLOCK_Pos)
#define DMA_CH_STATIC4_WR_PER_NUM(v) \
	((v & 0x7F) << DMA_CH_STATIC4_WR_PER_NUM_Pos)
#define DMA_CH_STATIC4_WR_PER_DELAY(v) \
	((v & 0x07) << DMA_CH_STATIC4_WR_PER_DELAY_Pos)
#define DMA_CH_STATIC4_WR_PER_BLOCK(v) \
	((v & 0x01) << DMA_CH_STATIC4_WR_PER_BLOCK_Pos)

#define DMA_CH_NDTL_BUFFER_SIZE_Pos 0

#define DMA_CH_NDTL_BUFFER_SIZE(v) ((v & 0x3FF) << DMA_CH_NDTL_BUFFER_SIZE_Pos)

#define DMA_CH_CONFIG_CMD_SET_INT_Pos 0
#define DMA_CH_CONFIG_CMD_LAST_Pos    1
#define DMA_CH_CONFIG_NEXT_ADDR_Pos   2

#define DMA_CH_CONFIG_CMD_SET_INT(v) ((v & 0x01) << DMA_CH_CONFIG_CMD_SET_INT_Pos)
#define DMA_CH_CONFIG_CMD_LAST(v)    ((v & 0x01) << DMA_CH_CONFIG_CMD_LAST_Pos)
#define DMA_CH_CONFIG_NEXT_ADDR(v)   ((v & 0xFFFFFFFC))

#define DMA_CH_INT_ENABLE_CH_END_Pos      0
#define DMA_CH_INT_ENABLE_RD_SLVERR_Pos   1
#define DMA_CH_INT_ENABLE_WR_SLVERR_Pos   2
#define DMA_CH_INT_ENABLE_RD_DECERR_Pos   3
#define DMA_CH_INT_ENABLE_WR_DECERR_Pos   4
#define DMA_CH_INT_ENABLE_OVERFLOW_Pos    5
#define DMA_CH_INT_ENABLE_UNDERFLOW_Pos   6
#define DMA_CH_INT_ENABLE_TIMEOUT_R_Pos   7
#define DMA_CH_INT_ENABLE_TIMEOUT_AR_Pos  8
#define DMA_CH_INT_ENABLE_TIMEOUT_B_Pos   9
#define DMA_CH_INT_ENABLE_TIMEOUT_W_Pos   10
#define DMA_CH_INT_ENABLE_TIMEOUT_AW_Pos  11
#define DMA_CH_INT_ENABLE_WDT_TIMEOUT_Pos 12

#define DMA_CH_INT_ENABLE_CH_END(v)    \
	((v & 0x01) << DMA_CH_INT_ENABLE_CH_END_Pos)
#define DMA_CH_INT_ENABLE_RD_SLVERR(v) \
	((v & 0x01) << DMA_CH_INT_ENABLE_RD_SLVERR_Pos)
#define DMA_CH_INT_ENABLE_WR_SLVERR(v) \
	((v & 0x01) << DMA_CH_INT_ENABLE_WR_SLVERR_Pos)
#define DMA_CH_INT_ENABLE_RD_DECERR(v) \
	((v & 0x01) << DMA_CH_INT_ENABLE_RD_DECERR_Pos)
#define DMA_CH_INT_ENABLE_WR_DECERR(v) \
	((v & 0x01) << DMA_CH_INT_ENABLE_WR_DECERR_Pos)
#define DMA_CH_INT_ENABLE_OVERFLOW(v) \
	((v & 0x01) << DMA_CH_INT_ENABLE_OVERFLOW_Pos)
#define DMA_CH_INT_ENABLE_UNDERFLOW(v) \
	((v & 0x01) << DMA_CH_INT_ENABLE_UNDERFLOW_Pos)
#define DMA_CH_INT_ENABLE_TIMEOUT_R(v) \
	((v & 0x01) << DMA_CH_INT_ENABLE_TIMEOUT_R_Pos)
#define DMA_CH_INT_ENABLE_TIMEOUT_AR(v) \
	((v & 0x01) << DMA_CH_INT_ENABLE_TIMEOUT_AR_Pos)
#define DMA_CH_INT_ENABLE_TIMEOUT_B(v) \
	((v & 0x01) << DMA_CH_INT_ENABLE_TIMEOUT_B_Pos)
#define DMA_CH_INT_ENABLE_TIMEOUT_W(v) \
	((v & 0x01) << DMA_CH_INT_ENABLE_TIMEOUT_W_Pos)
#define DMA_CH_INT_ENABLE_TIMEOUT_AW(v) \
	((v & 0x01) << DMA_CH_INT_ENABLE_TIMEOUT_AW_Pos)
#define DMA_CH_INT_ENABLE_WDT_TIMEOUT(v) \
	((v & 0x01) << DMA_CH_INT_ENABLE_WDT_TIMEOUT_Pos)

struct niiet_dma_regs {
	struct niiet_dma_ch_regs CH[NIIET_DMA_CH_MAX];
	/*!< DMA Core Interrupt status register */
	volatile uint32_t CH_INTSTAT;
	/*!< DMA Core Control register */
	volatile uint32_t CONTROL;
	/*!< DMA Core read priority channels */
	volatile uint32_t RD_PRIORITY;
	/*!< DMA Core write priority channels */
	volatile uint32_t WR_PRIORITY;
	/*!< DMA channel start register */
	volatile uint32_t CH_START;
	/*!< DMA channel enable register */
	volatile uint32_t CH_ENABLE;
	/*!< Idle indication register */
	volatile uint32_t IDLE;

	volatile uint32_t Reserved0;

	/*!< Direct control of peripheral RX request (register 0) */
	volatile uint32_t PER_RX0_CTRL;
	/*!< Direct control of peripheral TX request (register 0) */
	volatile uint32_t PER_TX0_CTRL;
	/*!< Direct control of peripheral RX request (register 1) */
	volatile uint32_t PER_RX1_CTRL;
	/*!< Direct control of peripheral TX request (register 1) */
	volatile uint32_t PER_TX1_CTRL;
	/*!< Direct control of peripheral RX request (register 2) */
	volatile uint32_t PER_RX2_CTRL;
	/*!< Direct control of peripheral TX request (register 2) */
	volatile uint32_t PER_TX2_CTRL;

	uint32_t Reserved1[2];
};

#define MEMORY_DMA_REQUESTOR_IDX  0
#define SPI0_DMA_REQUESTOR_IDX    1
#define SPI1_DMA_REQUESTOR_IDX    2
#define SPI2_DMA_REQUESTOR_IDX    3
#define SPI3_DMA_REQUESTOR_IDX    4
#define SPI4_DMA_REQUESTOR_IDX    5
#define SPI5_DMA_REQUESTOR_IDX    6
#define SPI6_DMA_REQUESTOR_IDX    7
#define SPI7_DMA_REQUESTOR_IDX    8
#define UART0_DMA_REQUESTOR_IDX   9
#define UART1_DMA_REQUESTOR_IDX   10
#define UART2_DMA_REQUESTOR_IDX   11
#define UART3_DMA_REQUESTOR_IDX   12
#define UART4_DMA_REQUESTOR_IDX   13
#define UART5_DMA_REQUESTOR_IDX   14
#define UART6_DMA_REQUESTOR_IDX   15
#define UART7_DMA_REQUESTOR_IDX   16
#define TMR0_DMA_REQUESTOR_IDX    17
#define TMR1_DMA_REQUESTOR_IDX    18
#define TMR2_DMA_REQUESTOR_IDX    19
#define TMR3_DMA_REQUESTOR_IDX    20
#define TMR4_DMA_REQUESTOR_IDX    21
#define TMR5_DMA_REQUESTOR_IDX    22
#define TMR6_DMA_REQUESTOR_IDX    23
#define TMR7_DMA_REQUESTOR_IDX    24
#define TMR8_DMA_REQUESTOR_IDX    25
#define TMR9_DMA_REQUESTOR_IDX    26
#define TMR10_DMA_REQUESTOR_IDX   27
#define TMR11_DMA_REQUESTOR_IDX   28
#define TMR12_DMA_REQUESTOR_IDX   29
#define TMR13_DMA_REQUESTOR_IDX   30
#define TMR14_DMA_REQUESTOR_IDX   31
#define TMR15_DMA_REQUESTOR_IDX   32
#define ADCSEQ0_DMA_REQUESTOR_IDX 33
#define ADCSEQ1_DMA_REQUESTOR_IDX 34
#define ADCSEQ2_DMA_REQUESTOR_IDX 35
#define ADCSEQ3_DMA_REQUESTOR_IDX 36
#define ADCSEQ4_DMA_REQUESTOR_IDX 37
#define ADCSEQ5_DMA_REQUESTOR_IDX 38
#define ADCSEQ6_DMA_REQUESTOR_IDX 39
#define ADCSEQ7_DMA_REQUESTOR_IDX 40
#define I2C0_DMA_REQUESTOR_IDX    41
#define I2C1_DMA_REQUESTOR_IDX    42
#define I2C2_DMA_REQUESTOR_IDX    43
#define I2C3_DMA_REQUESTOR_IDX    44
#define GPIO0_DMA_REQUESTOR_IDX   45
#define GPIO1_DMA_REQUESTOR_IDX   46
#define GPIO2_DMA_REQUESTOR_IDX   47
#define GPIO3_DMA_REQUESTOR_IDX   48
#define GPIO4_DMA_REQUESTOR_IDX   49
#define GPIO5_DMA_REQUESTOR_IDX   50
#define GPIO6_DMA_REQUESTOR_IDX   51
#define HASH_DMA_REQUESTOR_IDX    52
#define PWM0_DMA_REQUESTOR_IDX    53
#define PWM1_DMA_REQUESTOR_IDX    54
#define PWM2_DMA_REQUESTOR_IDX    55
#define PWM3_DMA_REQUESTOR_IDX    56
#define PWM4_DMA_REQUESTOR_IDX    57
#define PWM5_DMA_REQUESTOR_IDX    58
#define PWM6_DMA_REQUESTOR_IDX    59
#define PWM7_DMA_REQUESTOR_IDX    60
#define PWM8_DMA_REQUESTOR_IDX    61
#define PWM9_DMA_REQUESTOR_IDX    62
#define PWM10_DMA_REQUESTOR_IDX   63
#define PWM11_DMA_REQUESTOR_IDX   64
#define PWM12_DMA_REQUESTOR_IDX   65
#define PWM13_DMA_REQUESTOR_IDX   66
#define PWM14_DMA_REQUESTOR_IDX   67
#define PWM15_DMA_REQUESTOR_IDX   68
#define DAC0_DMA_REQUESTOR_IDX    69
#define DAC1_DMA_REQUESTOR_IDX    70

#endif /* DRIVERS_DMA_NIIET_DMA_REGS_H_ */

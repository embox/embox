/**
 * @file
 *
 * @date Sep 25, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_I2C_ADAPTERS_I2C_DESIGNWARE_I2C_DESIGNWARE_H_
#define SRC_DRIVERS_I2C_ADAPTERS_I2C_DESIGNWARE_I2C_DESIGNWARE_H_

#include <stdint.h>

#define I2C_DW_IC_CON_MASTER           0x1
#define I2C_DW_IC_CON_SPEED_STD        0x2
#define I2C_DW_IC_CON_SPEED_FAST       0x4
#define I2C_DW_IC_CON_SPEED_HIGH       0x6
#define I2C_DW_IC_CON_SPEED_MASK       0x6
#define I2C_DW_IC_CON_10BITADDR_MASTER 0x10
#define I2C_DW_IC_CON_RESTART_EN       0x20
#define I2C_DW_IC_CON_SLAVE_DISABLE    0x40

#define ACCESS_SWAP      0x00000001
#define ACCESS_16BIT     0x00000002
#define ACCESS_INTR_MASK 0x00000004

/*
 * status codes
 */
#define STATUS_IDLE              0x0
#define STATUS_WRITE_IN_PROGRESS 0x1
#define STATUS_READ_IN_PROGRESS  0x2
#define STATUS_IN_PROGRESS       0x4

struct i2c_dw_dev {
	int irq_num;
	uintptr_t base_addr;

	uint32_t accessor_flags;
	uint32_t master_cfg;
	uint32_t tx_fifo_depth;
	uint32_t rx_fifo_depth;
	uint32_t abort_source;
	uint32_t cmd_err;

	struct i2c_msg *msgs;

	int msgs_num;

	int msg_write_idx;
	uint32_t tx_buf_len;
	uint8_t *tx_buf;

	int msg_read_idx;
	uint32_t rx_buf_len;
	uint8_t *rx_buf;

	int msg_err;
	unsigned int status;

	int rx_outstanding;
};

extern const struct i2c_ops i2c_dw_ops;

extern int i2c_dw_master_init(struct i2c_dw_dev *dev);

/*
 * Registers offset
 */
#define I2C_DW_IC_CON               0x0
#define I2C_DW_IC_TAR               0x4
#define I2C_DW_IC_DATA_CMD          0x10
#define I2C_DW_IC_SS_SCL_HCNT       0x14
#define I2C_DW_IC_SS_SCL_LCNT       0x18
#define I2C_DW_IC_FS_SCL_HCNT       0x1c
#define I2C_DW_IC_FS_SCL_LCNT       0x20
#define I2C_DW_IC_HS_SCL_HCNT       0x24
#define I2C_DW_IC_HS_SCL_LCNT       0x28
#define I2C_DW_IC_INTR_STAT         0x2c
#define I2C_DW_IC_INTR_MASK         0x30
#define I2C_DW_IC_RAW_INTR_STAT     0x34
#define I2C_DW_IC_RX_TL             0x38
#define I2C_DW_IC_TX_TL             0x3c
#define I2C_DW_IC_CLR_INTR          0x40
#define I2C_DW_IC_CLR_RX_UNDER      0x44
#define I2C_DW_IC_CLR_RX_OVER       0x48
#define I2C_DW_IC_CLR_TX_OVER       0x4c
#define I2C_DW_IC_CLR_RD_REQ        0x50
#define I2C_DW_IC_CLR_TX_ABRT       0x54
#define I2C_DW_IC_CLR_RX_DONE       0x58
#define I2C_DW_IC_CLR_ACTIVITY      0x5c
#define I2C_DW_IC_CLR_STOP_DET      0x60
#define I2C_DW_IC_CLR_START_DET     0x64
#define I2C_DW_IC_CLR_GEN_CALL      0x68
#define I2C_DW_IC_ENABLE            0x6c
#define I2C_DW_IC_STATUS            0x70
#define I2C_DW_IC_TXFLR             0x74
#define I2C_DW_IC_RXFLR             0x78
#define I2C_DW_IC_SDA_HOLD          0x7c
#define I2C_DW_IC_TX_ABRT_SOURCE    0x80
#define I2C_DW_IC_ENABLE_STATUS     0x9c
#define I2C_DW_IC_COMP_PARAM_1      0xf4
#define I2C_DW_IC_COMP_VERSION      0xf8
#define I2C_DW_IC_SDA_HOLD_MIN_VERS 0x3131312A
#define I2C_DW_IC_COMP_TYPE         0xfc
#define I2C_DW_IC_COMP_TYPE_VALUE   0x44570140

#define I2C_DW_IC_INTR_RX_UNDER  0x001
#define I2C_DW_IC_INTR_RX_OVER   0x002
#define I2C_DW_IC_INTR_RX_FULL   0x004
#define I2C_DW_IC_INTR_TX_OVER   0x008
#define I2C_DW_IC_INTR_TX_EMPTY  0x010
#define I2C_DW_IC_INTR_RD_REQ    0x020
#define I2C_DW_IC_INTR_TX_ABRT   0x040
#define I2C_DW_IC_INTR_RX_DONE   0x080
#define I2C_DW_IC_INTR_ACTIVITY  0x100
#define I2C_DW_IC_INTR_STOP_DET  0x200
#define I2C_DW_IC_INTR_START_DET 0x400
#define I2C_DW_IC_INTR_GEN_CALL  0x800

#define I2C_DW_IC_INTR_DEFAULT_MASK                                            \
	(I2C_DW_IC_INTR_RX_FULL | I2C_DW_IC_INTR_TX_EMPTY | I2C_DW_IC_INTR_TX_ABRT \
	    | I2C_DW_IC_INTR_STOP_DET)

#define I2C_DW_IC_STATUS_ACTIVITY 0x1

#define I2C_DW_IC_SDA_HOLD_RX_SHIFT 16
#define I2C_DW_IC_SDA_HOLD_RX_MASK  0x00FF0000

#define I2C_DW_IC_ERR_TX_ABRT 0x1

#define I2C_DW_IC_TAR_10BITADDR_MASTER (1 << 12)

#define I2C_DW_IC_COMP_PARAM_1_SPEED_MODE_HIGH ((1 << 2) | (1 << 3))
#define I2C_DW_IC_COMP_PARAM_1_SPEED_MODE_MASK (0x3 << 2)

#endif /* SRC_DRIVERS_I2C_ADAPTERS_I2C_DESIGNWARE_I2C_DESIGNWARE_H_ */

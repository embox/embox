#ifndef ESP32_I2C_H_
#define ESP32_I2C_H_

/* ESP32-C5 HP I2C register offsets and fields. */
#define ESP32_I2C_SCL_LOW              0x00
#define ESP32_I2C_CTR                  0x04
#define ESP32_I2C_SR                   0x08
#define ESP32_I2C_TIMEOUT              0x0c
#define ESP32_I2C_SLAVE_ADDR           0x10
#define ESP32_I2C_FIFO_ST              0x14
#define ESP32_I2C_FIFO_CONF            0x18
#define ESP32_I2C_DATA                 0x1c
#define ESP32_I2C_INT_RAW              0x20
#define ESP32_I2C_INT_CLR              0x24
#define ESP32_I2C_INT_ENA              0x28
#define ESP32_I2C_INT_STATUS           0x2c
#define ESP32_I2C_SDA_HOLD             0x30
#define ESP32_I2C_SDA_SAMPLE           0x34
#define ESP32_I2C_SCL_HIGH             0x38
#define ESP32_I2C_START_HOLD           0x40
#define ESP32_I2C_RESTART_SETUP        0x44
#define ESP32_I2C_STOP_HOLD            0x48
#define ESP32_I2C_STOP_SETUP           0x4c
#define ESP32_I2C_FILTER_CFG           0x50
#define ESP32_I2C_CLK_CONF             0x54
#define ESP32_I2C_COMD0                0x58
#define ESP32_I2C_COMD1                0x5c
#define ESP32_I2C_COMD2                0x60
#define ESP32_I2C_COMD3                0x64
#define ESP32_I2C_COMD4                0x68
#define ESP32_I2C_COMD5                0x6c
#define ESP32_I2C_COMD6                0x70
#define ESP32_I2C_COMD7                0x74
#define ESP32_I2C_SCL_ST_TIME_OUT      0x78
#define ESP32_I2C_SCL_MAIN_ST_TIME_OUT 0x7c
#define ESP32_I2C_SCL_SP_CONF          0x80
#define ESP32_I2C_SCL_STRETCH_CONF     0x84
#define ESP32_I2C_DATE                 0xf8
#define ESP32_I2C_TXFIFO_START_ADDR    0x100
#define ESP32_I2C_RXFIFO_START_ADDR    0x180

#define ESP32_I2C_CTR_MASTER           (1U << 4)
#define ESP32_I2C_CTR_TRANS_START      (1U << 5)
#define ESP32_I2C_CTR_CLK_EN           (1U << 8)
#define ESP32_I2C_CTR_FSM_RST          (1U << 10)
#define ESP32_I2C_CTR_CONF_UPDATE      (1U << 11)
#define ESP32_I2C_TIMEOUT_EN           (1U << 5)
#define ESP32_I2C_FIFO_RX_RST          (1U << 12)
#define ESP32_I2C_FIFO_TX_RST          (1U << 13)
#define ESP32_I2C_FIFO_PRT_EN          (1U << 14)
#define ESP32_I2C_FILTER_SCL_EN        (1U << 8)
#define ESP32_I2C_FILTER_SDA_EN        (1U << 9)
#define ESP32_I2C_END_DETECT_INT       (1U << 3)
#define ESP32_I2C_INT_MASK             0x7ffffU
#define ESP32_I2C_TRANS_COMPLETE_INT   (1U << 7)
#define ESP32_I2C_TIME_OUT_INT         (1U << 8)
#define ESP32_I2C_NACK_INT             (1U << 10)
#define ESP32_I2C_ARBITRATION_LOST_INT (1U << 5)
#define ESP32_I2C_SCL_ST_TO_INT        (1U << 13)
#define ESP32_I2C_SCL_MAIN_ST_TO_INT   (1U << 14)

#define ESP32_I2C_FIFO_LEN 32U

#define ESP32_I2C_CMD_WRITE  (1U << 11)
#define ESP32_I2C_CMD_STOP   (2U << 11)
#define ESP32_I2C_CMD_READ   (3U << 11)
#define ESP32_I2C_CMD_END    (4U << 11)
#define ESP32_I2C_CMD_RSTART (6U << 11)

/* Combine command fields with an opcode using bitwise OR.
 * ACK_VAL: ACK/NACK sent on reads; ACK_EXP: expected ACK/NACK on writes.
 * A cleared ACK bit means ACK, a set bit means NACK.
 */
#define ESP32_I2C_CMD_BYTE_NUM(n)  ((n) & 0xffU)
#define ESP32_I2C_CMD_ACK_CHECK_EN (1U << 8)
#define ESP32_I2C_CMD_ACK_EXP      (1U << 9)
#define ESP32_I2C_CMD_ACK_VAL      (1U << 10)
#define ESP32_I2C_CMD_DONE         (1U << 31)

#endif /* ESP32_I2C_H_ */

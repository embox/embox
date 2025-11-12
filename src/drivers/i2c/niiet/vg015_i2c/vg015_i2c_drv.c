/**
 * @file
 *
 * @date Oct 23, 2025
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <drivers/i2c/i2c.h>


#define SMBUS_SYSTEM_FREQ 16000000
#define SMBUS_BLOCK_BYTES_MAX 32

#define FSFreq 100000
#define HSFreq 3400000

struct niiet_i2c_regs {                                                            
	volatile uint32_t SDA;                                               /*!< Data register */
	volatile uint32_t ST;                                                /*!< Status register */
	volatile uint32_t CST;                                               /*!< Status and control register */
 	volatile uint32_t CTL0;                                              /*!< Control register 0 */                                                   
    volatile uint32_t ADDR;                                              /*!< Register own address */
    volatile uint32_t CTL1;                                              /*!< Control register 1 */
	volatile uint32_t TOPR;                                              /*!< Prescaler load register */
	volatile uint32_t CTL2;                                              /*!< Control register 2 */
	volatile uint32_t CTL3;                                              /*!< Control register 3 */
	volatile uint32_t CTL4;                                              /*!< Control Register 4 */
};


#define CTL0_START_OFFSET     0x0
#define CTL0_STOP_OFFSET      0x1
#define CTL0_INTEN_OFFSET     0x2
#define CTL0_ACK_OFFSET       0x4
#define CTL0_GCMEN_OFFSET     0x5
#define CTL0_SMBARE_OFFSET    0x6
#define CTL0_CLRST_OFFSET     0x7

#define CTL0_START_BIT        (1 << CTL0_START_OFFSET)
#define CTL0_STOP_BIT         (1 << CTL0_STOP_OFFSET)
#define CTL0_INTEN_BIT        (1 << CTL0_INTEN_OFFSET)
#define CTL0_ACK_BIT          (1 << CTL0_ACK_OFFSET)
#define CTL0_GCMEN_BIT        (1 << CTL0_GCMEN_OFFSET)
#define CTL0_SMBARE_BIT       (1 << CTL0_SMBARE_OFFSET)
#define CTL0_CLRST_BIT        (1 << CTL0_CLRST_OFFSET)

#define CTL1_ENABLE_OFFSET     0x0
#define CTL1_SCLFRQ_OFFSET     0x1

#define CTL1_ENABLE_BIT        (1 << CTL1_ENABLE_OFFSET)
#define CTL1_SCLFRQ(freq)      ((freq & 0x7F) << CTL1_SCLFRQ_OFFSET)

#define CTL3_SCLFRQ(freq)      (freq & 0xFF)

#define CTL2_SHSDIV_OFFSET     0x4

#define CTL2_SHSDIV(freq)      ((freq & 0x0F) << CTL2_SHSDIV_OFFSET)

#define CTL4_SHSDIV(freq)      (freq & 0xFF)

#define ST_MODE_OFFSET         0x0        
#define ST_INT_OFFSET          0x7

#define ST_INT_BIT             (1 << ST_INT_OFFSET)
#define ST_MODE(reg)           ((reg & 0x3F) << ST_MODE_OFFSET)

#define CST_BB_OFFSET          0x0

#define CST_BB_BIT             (1 << CST_BB_OFFSET)

#define I2C_ST_MODE_IDLE       0x0UL      /*!< General - Idle, no valid status information available */
#define I2C_ST_MODE_STDONE     0x1UL      /*!< FS master - Start condition generated */
#define I2C_ST_MODE_RSDONE     0x2UL      /*!< FS master - Repeated start condition generated */
#define I2C_ST_MODE_IDLARL     0x3UL      /*!< FS master - Arbitration lost, unaddressed slave mode entered */
#define I2C_ST_MODE_MTADPA     0x4UL      /*!< FS master transmit - Slave address sent, positive ACK */
#define I2C_ST_MODE_MTADNA     0x5UL      /*!< FS master transmit - Slave address sent, negative ACK */
#define I2C_ST_MODE_MTDAPA     0x6UL      /*!< FS master transmit - Data byte sent, positive ACK */
#define I2C_ST_MODE_MTDANA     0x7UL      /*!< FS master transmit - Data byte sent, negative ACK */
#define I2C_ST_MODE_MRADPA     0x8UL      /*!< FS master receive - Slave addres sent, positive ACK */
#define I2C_ST_MODE_MRADNA     0x9UL      /*!< FS master receive - Slave addres sent, negative ACK */
#define I2C_ST_MODE_MRDAPA     0xAUL      /*!< FS master receive - Data byte received, positive ACK */
#define I2C_ST_MODE_MRDANA     0xBUL      /*!< FS master receive - Data byte received, negative ACK */
#define I2C_ST_MODE_MTMCER     0xCUL      /*!< FS master - Mastercode transmitted, error detected (positive ACK) */
#define I2C_ST_MODE_SRADPA     0x10UL     /*!< FS slave receive - Slave address received, positive ACK */
#define I2C_ST_MODE_SRAAPA     0x11UL     /*!< FS slave receive - Slave address received after arbitration loss, positive ACK */
#define I2C_ST_MODE_SRDAPA     0x12UL     /*!< FS slave receive - Data byte received, positive ACK */
#define I2C_ST_MODE_SRDANA     0x13UL     /*!< FS slave receive - Data byte received, negative ACK */
#define I2C_ST_MODE_STADPA     0x14UL     /*!< FS slave transmit - Slave address received, positive ACK */
#define I2C_ST_MODE_STAAPA     0x15UL     /*!< FS slave transmit - Slave address received, negative ACK */
#define I2C_ST_MODE_STDAPA     0x16UL     /*!< FS slave transmit - Data byte sent, positive ACK */
#define I2C_ST_MODE_STDANA     0x17UL     /*!< FS slave transmit - Data byte sent, negative ACK */
#define I2C_ST_MODE_SATADP     0x18UL     /*!< FS slave transmit alert response - Alert response address received, positive ACK */
#define I2C_ST_MODE_SATAAP     0x19UL     /*!< FS slave transmit alert response - Alert response address received after arbitration loss, positive ACK */
#define I2C_ST_MODE_SATDAP     0x1AUL     /*!< FS slave transmit alert response - Alert response data byte sent, positive ACK */
#define I2C_ST_MODE_SATDAN     0x1BUL     /*!< FS slave transmit alert response - Alert response data byte sent, negative ACK */
#define I2C_ST_MODE_SSTOP      0x1CUL     /*!< FS slave - Slave mode stop condition detected */
#define I2C_ST_MODE_SGADPA     0x1DUL     /*!< FS slave - Global call address received, positive ACK */
#define I2C_ST_MODE_SDAAPA     0x1EUL     /*!< FS slave - Global call address received after arbitration loss, positive ACK */
#define I2C_ST_MODE_BERROR     0x1FUL     /*!< General - Bus error detected (invalid start or stop condition */
#define I2C_ST_MODE_HMTMCOK    0x21UL     /*!< HS master - Master code transmitted OK - switched to HS mode */
#define I2C_ST_MODE_HRSDONE    0x22UL     /*!< HS master - Repeated start condition generated */
#define I2C_ST_MODE_HIDLARL    0x23UL     /*!< HS master - Arbitration lost, HS unaddressed slave mode entered */
#define I2C_ST_MODE_HMTADPA    0x24UL     /*!< HS master transmit - Slave address sent, positive ACK */
#define I2C_ST_MODE_HMTADNA    0x25UL     /*!< HS master transmit - Slave address sent, negative ACK */
#define I2C_ST_MODE_HMTDAPA    0x26UL     /*!< HS master transmit - Data byte sent, positive ACK */
#define I2C_ST_MODE_HMTDANA    0x27UL     /*!< HS master transmit - Data byte sent, negative ACK */
#define I2C_ST_MODE_HMRADPA    0x28UL     /*!< HS master receive - Slave address sent, positive ACK */
#define I2C_ST_MODE_HMRADNA    0x29UL     /*!< HS master receive - Slave address sent, negative ACK */
#define I2C_ST_MODE_HMRDAPA    0x2AUL     /*!< HS master receive - Data byte received, positive ACK */
#define I2C_ST_MODE_HMRDANA    0x2BUL     /*!< HS master receive - Data byte received, negative ACK */
#define I2C_ST_MODE_HSRADPA    0x30UL     /*!< HS slave receive - Slave address received, positive ACK */
#define I2C_ST_MODE_HSRDAPA    0x32UL     /*!< HS slave receive - Data byte received, positive ACK */
#define I2C_ST_MODE_HSRDANA    0x33UL     /*!< HS slave receive - Data byte received, negative ACK */
#define I2C_ST_MODE_HSTADPA    0x34UL     /*!< HS slave transmit - Slave address received, positive ACK */
#define I2C_ST_MODE_HSTDAPA    0x36UL     /*!< HS slave transmit - Data byte sent, positive ACK */
#define I2C_ST_MODE_HSTDANA    0x37UL     /*!< HS slave transmit - Data byte sent, negative ACK */



static int vg015_i2c_tx(const struct i2c_bus *bus, uint16_t addr,
									uint8_t *buff, size_t sz) {
	struct niiet_i2c_regs *regs;
	int i;

	regs = (void *)bus->i2cb_label;

	while(!(regs->ST & ST_INT_BIT)) ;
	if (ST_MODE(regs->ST) & I2C_ST_MODE_STDONE) {
		regs->SDA = (addr << 1) | 1;
	} else {
		log_error("st mode not I2C_ST_MODE_STDONE");
	}

	for (i = 0; i < sz; i ++) {
		if(regs->ST & ST_INT_BIT) {
			uint32_t mode;

			mode = ST_MODE(regs->ST);
			switch (mode) {
				case I2C_ST_MODE_MTADPA:
					regs->SDA = buff[i];
					break;
				case I2C_ST_MODE_MTDAPA:
					regs->CTL0 |= CTL0_STOP_BIT;

					break;
				default  :	break;
			}

			regs->CTL0 = (CTL0_CLRST_BIT /*| CTL0_INTEN_BIT */);
		}
	}

	return 0;
}

static int vg015_i2c_rx(const struct i2c_bus *bus, uint16_t addr,
									uint8_t *buff, size_t sz) {
	struct niiet_i2c_regs *regs;
	int i;

	regs = (void *)bus->i2cb_label;

	while(!(regs->ST & ST_INT_BIT)) ;
	if (ST_MODE(regs->ST) & I2C_ST_MODE_STDONE) {
		regs->SDA = (addr << 1) | 1;
	} else {
		log_error("st mode not I2C_ST_MODE_STDONE");
	}

	for (i = 0; i < sz; i ++) {
		if(regs->ST & ST_INT_BIT) {
			uint32_t mode;

			mode = ST_MODE(regs->ST);
			switch (mode) {
				case I2C_ST_MODE_MRADPA:
					break;
				case I2C_ST_MODE_MRDAPA:
					buff[i] = regs->SDA;
					if (i != sz - 1) {
						regs->CTL0 |= CTL0_ACK_BIT;
					}/* else {
						regs->CTL0 &= ~CTL0_ACK_BIT;
					} */

					break;
				case I2C_ST_MODE_MRDANA:
					regs->CTL0 |= CTL0_STOP_BIT;
					break;
				default  :	break;
			}

			regs->CTL0 = (CTL0_CLRST_BIT /*| CTL0_INTEN_BIT */);
		}
	}

	return 0;
}

static inline int niiet_i2c_is_busy(struct niiet_i2c_regs *regs) {
	return regs->CST & CST_BB_BIT;

}

static int vg015_i2c_master_xfer(const struct i2c_bus *bus,
						struct i2c_msg *msgs, size_t num_msgs) {
	struct niiet_i2c_regs *regs;
	int res;
	int i;

	regs = (void *)bus->i2cb_label;

	i = 0;
	while(niiet_i2c_is_busy(regs)) {
		if (i++ > 1000) {
			return -EBUSY;
		}
	}

	regs->CTL0 = CTL0_START_BIT;


	for (i = 0; i < num_msgs; i++) {
		if (msgs[i].flags & I2C_M_RD) {
			res = vg015_i2c_rx(bus, msgs->addr, msgs[i].buf, msgs[i].len);
		}
		else {
			res = vg015_i2c_tx(bus, msgs->addr, msgs[i].buf, msgs[i].len);
		}
	}

	return res;
}

extern int vg015_i2c_hw_init0(const struct i2c_bus *bus);

static int vg015_i2c_init(const struct i2c_bus *bus) {
	struct niiet_i2c_regs *regs;
	uint32_t freq_calc;

	regs = (void *)bus->i2cb_label;

	vg015_i2c_hw_init0(bus);

	freq_calc = SMBUS_SYSTEM_FREQ / (4 * FSFreq);
	regs->CTL1 = CTL1_SCLFRQ(freq_calc & 0x7F);
	regs->CTL3 = CTL3_SCLFRQ(freq_calc >> 7);

	freq_calc = SMBUS_SYSTEM_FREQ / ( 3 * HSFreq );
	regs->CTL2 = CTL2_SHSDIV(freq_calc & 0x0F);
	regs->CTL4 = CTL4_SHSDIV(freq_calc >> 4);

	regs->CTL1 |= CTL1_ENABLE_BIT;

	return 0;
}

const struct i2c_ops vg015_i2c_ops = {
    .i2c_master_xfer = vg015_i2c_master_xfer,
    .i2c_init = vg015_i2c_init,
};

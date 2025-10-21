/**
 * @file
 * @brief
 * @author Andrew Bursian
 * @version
 * @date 20.06.2023
 */
#ifndef SRC_DRIVERS_SPI_STM32_I2C_H_
#define SRC_DRIVERS_SPI_STM32_I2C_H_

struct skeleton_spi {
	void *instance;
};

extern int skeleton_spi_init(struct skeleton_spi *dev);

extern struct spi_controller_ops skeleton_spic_ops;

#endif /* SRC_DRIVERS_SPI_STM32_I2C_H_ */

#ifndef PLATFORM_ADAR7251_DRIVERS_ADAR7251_DRIVER_H_
#define PLATFORM_ADAR7251_DRIVERS_ADAR7251_DRIVER_H_

struct spi_device;
struct sai_device;

struct adar7251_dev {
	int spi_bus;
	struct spi_device *spi_dev;
	struct sai_device *sai_dev;
};

extern int adar7251_hw_init(struct adar7251_dev *dev);

/* write down batch of registers */
extern void adar_preset_pll(struct adar7251_dev *dev);

/* write down batch of registers */
extern void adar_preset(struct adar7251_dev *dev);

#endif /* PLATFORM_ADAR7251_DRIVERS_ADAR7251_DRIVER_H_ */

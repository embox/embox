#ifndef _I2C_DRIVER
#define _I2C_DRIVER

#include <stddef.h>
#include <plib035_i2c.h>
#include <plib035_gpio.h>

#define I2C_DRIVER_SCL        (GPIO_Pin_0)
#define I2C_DRIVER_SDA        (GPIO_Pin_1)
#define I2C_DRIVER_PORT       (GPIOA)

#define I2C_DRIVER_READ_FLAG  1
#define I2C_DRIVER_WRITE_FLAG 0

/**
 * State of the I2C driver.
 */
typedef enum __attribute__((__packed__)) {
    I2C_DRIVER_BUSY = 0,     /**< Operations still in progress */
    I2C_DRIVER_OK,           /**< All operations successful */
    I2C_DRIVER_DEVICE_ERROR, /**< Error encountered in one of the operations */
    I2C_DRIVER_BUS_ERROR     /**< Error encountered in one of the operations */
} I2C_driver_state_t;

/**
 * Read or write I2C operation.
 */
typedef struct {
    uint8_t address; /**< Shifted device address. Lowest bit is responsible for the type of the operation -- 0 to write, 1 to read */
    uint8_t size;    /**< Number of bytes to read or write */
    uint8_t start;   /**< Start of the buffer, usually 0 */
    uint8_t* data;   /**< Buffer to read from or write to. Must be at least start + size bytes */
} I2C_driver_operation_t;

// Initialization:

/**
 * @brief Initializes the I2C block and sets up clocking, configures GPIO and interrupts.
 *
 * @param freq I2C bus frequency
 */
void I2C_driver_init(uint32_t freq);

// Low-level API:

/**
 * @brief Executes the operations in order, performing repeated start between them.
 *
 * @param operations Array of operations
 * @param size Length of the array of operations
 */
void I2C_driver_execute(I2C_driver_operation_t* operations, uint8_t size);

/**
 * @brief Checks the status of the executing operations.
 *
 * @return Return code.
 */
I2C_driver_state_t I2C_driver_is_done();

// Slightly higher-level API:

/**
 * @brief Executes one write operation.
 *
 * @param address Device address (not shifted)
 * @param data Buffer to read from
 * @param size Length of the buffer
 */
void I2C_driver_write(uint8_t address, const uint8_t* data, uint8_t size);

/**
 * @brief Executes one read operation.
 *
 * @param address Device address (not shifted)
 * @param data Buffer to write the data to
 * @param size Length of the buffer
 */
void I2C_driver_read(uint8_t address, uint8_t* data, uint8_t size);
// Still need is_done for it though

void I2C_IRQHandler();

I2C_driver_state_t I2C_driver_recover_from_error();

#endif //_I2C_DRIVER

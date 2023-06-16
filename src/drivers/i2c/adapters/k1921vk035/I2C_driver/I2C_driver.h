/*==============================================================================
 * Драйвер интерфейса I2C для МК К1921ВК035
 *------------------------------------------------------------------------------
 * ДЦЭ Vostok г.Владивосток 2022г., Лебедев Михаил <micha030201@gmail.com>,
 * ДЦЭ Vostok г.Владивосток 2022г., Войтенко Алексей <alexeyvoytenko.2000@gmail.com>,
 *==============================================================================
 */

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
 * Код возврата операций I2C.
 */
typedef enum __attribute__((__packed__)) {
    I2C_DRIVER_BUSY = 0, /**< Операция ещё выполняется */
    I2C_DRIVER_OK,       /**< Операция выполнилась успешно */
    I2C_DRIVER_ERROR     /**< Операция выполнилась с ошибкой */
} I2C_driver_state_t;

/**
 * Операция чтения или записи на шине I2C.
 */
typedef struct {
    uint8_t address; /**< Сдвинутый адрес устройства. Младший бит отвечает за тип операции -- 0 запись, 1 чтение */
    uint8_t size;    /**< Количество байт для чтения или записи */
    uint8_t start;   /**< Начало буфера чтения или записи, обычно 0 */
    uint8_t* data; /**< Буфер чтения или записи. Должен быть размером как минимум start + size */
} I2C_driver_operation_t;

// Initialization:

/**
 * @brief Инициализирует тактирование блока I2C, настраивает GPIO и прерывания.
 *
 * @param freq Частота шины I2C
 */
void I2C_driver_init(uint32_t freq);

// Low-level API:

/**
 * @brief Последовательно выполняет операции чтения и/или записи, выполняя повторный старт между операциями.
 *
 * @param operations Массив операций
 * @param size Размер массива операций
 */
void I2C_driver_execute(I2C_driver_operation_t* operations, uint8_t size);

/**
 * @brief Проверяет состояние выполнения операций.
 *
 * @return Код возврата.
 */
I2C_driver_state_t I2C_driver_is_done();

// Slightly higher-level API:

/**
 * @brief Производит одну операцию чтения.
 *
 * @param address Несдвинутый адрес устройства
 * @param data Буфер данных для записи
 * @param size Размер буфера данных для записи
 */
void I2C_driver_write(uint8_t address, const uint8_t* data, uint8_t size);

/**
 * @brief Производит одну операцию записи.
 *
 * @param address Несдвинутый адрес устройства
 * @param data Буфер для прочитанных данных
 * @param size Размер буфера для прочитанных данных
 */
void I2C_driver_read(uint8_t address, uint8_t* data, uint8_t size);
// Still need is_done for it though

void I2C_IRQHandler();

#endif //_I2C_DRIVER

#include "I2C_driver.h"

typedef struct {
    uint8_t operations_size;
    uint8_t operations_start;
    I2C_driver_state_t state;
    I2C_driver_operation_t* operations;
} I2C_driver_t;

static I2C_driver_t I2C_driver;

void I2C_driver_init(uint32_t freq) {
    RCU_AHBClkCmd(RCU_AHBClk_GPIOA, ENABLE);
    RCU_AHBRstCmd(RCU_AHBRst_GPIOA, ENABLE);

    RCU_APBClkCmd(RCU_APBClk_I2C, ENABLE);
    RCU_APBRstCmd(RCU_APBRst_I2C, ENABLE);

    GPIO_Init_TypeDef gpio_i2c_init_struct;
    GPIO_StructInit(&gpio_i2c_init_struct);
    gpio_i2c_init_struct.Pin = I2C_DRIVER_SCL | I2C_DRIVER_SDA;
    gpio_i2c_init_struct.AltFunc = ENABLE;
    gpio_i2c_init_struct.Out = ENABLE;
    gpio_i2c_init_struct.OutMode = GPIO_OutMode_OD;
    gpio_i2c_init_struct.PullMode = GPIO_PullMode_PU;
    gpio_i2c_init_struct.InMode = GPIO_InMode_Schmitt;
    gpio_i2c_init_struct.Digital = ENABLE;
    GPIO_Init(I2C_DRIVER_PORT, &gpio_i2c_init_struct);

    I2C_FSFreqConfig(freq, SystemCoreClock);
    I2C_Cmd(ENABLE);
    I2C_ITCmd(ENABLE);
    I2C_SlaveCmd(DISABLE);
}

void restart_or_stop() {
    // TODO if state == ERROR
    if (++I2C_driver.operations_start == I2C_driver.operations_size) {
        I2C_StopCmd();
        I2C_driver.state = I2C_DRIVER_OK;
    } else {
        I2C_StartCmd();
        I2C_driver.state = I2C_DRIVER_BUSY;
    }
}

void I2C_IRQHandler() {
    switch (I2C_GetState()) {
        case I2C_State_STDONE: // start/
        case I2C_State_RSDONE: // repeated start
            I2C_SetData(I2C_driver.operations[I2C_driver.operations_start].address);
            I2C_driver.state = I2C_DRIVER_BUSY;
            break;
        case I2C_State_MTADPA: // writing -- device address sent, ack
            if (I2C_driver.operations[I2C_driver.operations_start].size) {
                I2C_SetData(I2C_driver.operations[I2C_driver.operations_start]
                                .data[I2C_driver.operations[I2C_driver.operations_start].start++]);
                I2C_driver.state = I2C_DRIVER_BUSY;
            } else {
                restart_or_stop();
            }
            break;
        case I2C_State_MTDAPA: // writing -- data byte sent, ack
            if (I2C_driver.operations[I2C_driver.operations_start].start
                != I2C_driver.operations[I2C_driver.operations_start].size) { // FIXME hard to read with this styleguide
                I2C_SetData(I2C_driver.operations[I2C_driver.operations_start]
                                .data[I2C_driver.operations[I2C_driver.operations_start].start++]);
                I2C_driver.state = I2C_DRIVER_BUSY;
            } else {
                restart_or_stop();
            }
            break;
        case I2C_State_MRADPA: // reading -- device address sent, ack
            if (I2C_driver.operations[I2C_driver.operations_start].size == 1) {
                I2C_NACKCmd();
                I2C_driver.state = I2C_DRIVER_BUSY;
            } else if (I2C_driver.operations[I2C_driver.operations_start].size == 0) {
                restart_or_stop();
                I2C_driver.state = I2C_DRIVER_OK;
            } else {
                I2C_driver.state = I2C_DRIVER_BUSY;
            }
            break;
        case I2C_State_MRDAPA: // reading -- data byte received, ack
            if (I2C_driver.operations[I2C_driver.operations_start].start
                != I2C_driver.operations[I2C_driver.operations_start].size) {
                I2C_driver.operations[I2C_driver.operations_start]
                    .data[I2C_driver.operations[I2C_driver.operations_start].start++] = I2C_GetData();
                if (I2C_driver.operations[I2C_driver.operations_start].start
                    == I2C_driver.operations[I2C_driver.operations_start].size - 1) {
                    I2C_NACKCmd();
                }
                I2C_driver.state = I2C_DRIVER_BUSY;
            } else {
                I2C_driver.state = I2C_DRIVER_ERROR;
            }
            break;
        case I2C_State_MRDANA: // reading -- data byte received, nack
            if (I2C_driver.operations[I2C_driver.operations_start].start
                == I2C_driver.operations[I2C_driver.operations_start].size - 1) {
                I2C_driver.operations[I2C_driver.operations_start]
                    .data[I2C_driver.operations[I2C_driver.operations_start].start++] = I2C_GetData();
                restart_or_stop();
            } else {
                I2C_driver.state = I2C_DRIVER_ERROR;
            }
            break;
        case I2C_State_IDLE: // idle, no information about status
        case I2C_State_IDLARL: // losing arbitrage
        case I2C_State_MTADNA: // writing -- device address sent, nack
        case I2C_State_MTDANA: // writing -- data byte sent, nack
        case I2C_State_MRADNA: // reading -- device address sent, nack
        case I2C_State_MTMCER: // speed change code sent, error
        default:
            I2C_StopCmd();
            I2C_driver.state = I2C_DRIVER_ERROR;
            break;
    }
    I2C_ITStatusClear();
}

// Low-level API:

void I2C_driver_execute(I2C_driver_operation_t* operations, uint8_t size) {
    I2C_driver.operations = operations;
    I2C_driver.operations_size = size;
    I2C_driver.operations_start = 0;
    I2C_driver.state = I2C_DRIVER_BUSY;

    I2C_StartCmd();
}

I2C_driver_state_t I2C_driver_is_done() {
    if (I2C_driver.state == I2C_DRIVER_OK) {
        if (I2C->CTL0_bit.STOP) {
            return I2C_DRIVER_BUSY;
        }
    }
    return I2C_driver.state;
}

// Slightly higher-level API:

void I2C_driver_write(uint8_t address, const uint8_t* data, uint8_t size) {
    I2C_driver_operation_t op;

    op.address = (address << 1) | I2C_DRIVER_WRITE_FLAG;
    op.data = (uint8_t*)data;
    op.size = size;
    op.start = 0;

    I2C_driver_execute(&op, 1);
}

void I2C_driver_read(uint8_t address, uint8_t* data, uint8_t size) {
    I2C_driver_operation_t op;

    op.address = (address << 1) | I2C_DRIVER_READ_FLAG;
    op.data = data;
    op.size = size;
    op.start = 0;

    I2C_driver_execute(&op, 1);
}

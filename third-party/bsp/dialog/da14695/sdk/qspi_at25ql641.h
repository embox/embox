/**
 * \addtogroup PLA_BSP_SYSTEM
 * \{
 * \addtogroup PLA_MEMORY
 *
 * \{
 */

/**
 ****************************************************************************************
 *
 * @file qspi_at25sq641.h
 *
 * @brief Driver for flash ADESTO AT25QL641
 *
 * Copyright (C) 2016-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _QSPI_ADESTO_AT25QL641_H_
#define _QSPI_ADESTO_AT25QL641_H_

#include "qspi_common.h"

/************************************************* JEDEC ID INFO *************************************************/

/**
 * \brief The Flash type JEDEC ID
 *
 * This is the first byte returned by the 0x9F command
 */
#define ADESTO_ID                                       0x1F

/**
 * \brief The Flash type JEDEC ID
 *
 * This is the second byte returned by the 0x9F command
 */
#define AT25QL_TYPE                                     0x43

/**
 * \brief The Flash density JEDEC ID
 *
 * This is the third byte returned by the 0x9F command
 */
#define AT25QL641_SIZE                                  0x17

/**************************************************** TIMINGS ****************************************************/

/* Flash power up/down timings in usec */
#define AT25QL641_POWER_DOWN_DELAY_US                   3 /* cannot have less than 3us delay */
#define AT25QL641_RELEASE_POWER_DOWN_DELAY_US           3 /* cannot have less than 3us delay */
#define AT25QL641_POWER_UP_DELAY_US                     10000

/************************************************ Opcodes SECTION ************************************************/

/* Status Register Opcodes */
#define AT25QL641_READ_STATUS_REGISTER_2_OPCODE         0x35
#define AT25QL641_WRITE_STATUS_REGISTER_2_OPCODE        0x31

/* Suspend/Resume Opcodes */
#define AT25QL641_ERASE_PROGRAM_SUSPEND_OPCODE          0x75
#define AT25QL641_ERASE_PROGRAM_RESUME_OPCODE           0x7A

/* Quad Read Opcode */
#define AT25QL641_FAST_READ_QUAD_OPCODE                 0xEB

/* Quad Page Program */
#define AT25QL641_QUAD_PAGE_PROGRAM_OPCODE              0x33

/********************************************** DRIVER GENERIC INFO **********************************************/

/* Quad Enable Bit Position - Status Register 2 */
#define AT25QL641_SR_2_QE_BIT_POS                       (1)
#define AT25QL641_SR_2_QE_MASK                          (1 << AT25QL641_SR_2_QE_BIT_POS)

/* Erase/Program Suspend Bit Position - Status Register 2 */
#define AT25QL641_SR_2_ESUS_BIT_POS                     (7)
#define AT25QL641_SR_2_ESUS_MASK                        (1 << AT25QL641_SR_2_ESUS_BIT_POS)

/* Extra/Mode Byte used for entering continuous read mode */
#define AT25QL641_EXTRA_BYTE_M7_M0                      0xA0

/************************************************* uCode SECTION *************************************************/

/*
 * This section is OBSOLETE in DA1469x Family products however it is maintained here for reference reasons as
 * this part is mandatory for DA14683 products.
 */

#if (dg_configFLASH_POWER_OFF == 1)
/**
 * \brief uCode for handling the QSPI FLASH activation from power off.
 */

const uint32_t at25ql641_ucode_wakeup[] = {
        0x11000001 | (((uint16_t)(AT25QL641_POWER_UP_DELAY_US*1000/62.5) & 0xFFFF) << 8),
        0xFFFF0000,
};
#elif (dg_configFLASH_POWER_DOWN == 1)
/**
 * \brief uCode for handling the QSPI FLASH release from power-down.
 */
const uint32_t at25ql641_ucode_wakeup[] = {
        0xAB000009 | (((uint16_t)(AT25QL641_RELEASE_POWER_DOWN_DELAY_US*1000/62.5) & 0xFFFF) << 8),
};
#else
/**
 * \brief uCode for handling the QSPI FLASH exit from the "Continuous Read Mode".
 */
const uint32_t at25ql641_ucode_wakeup[] = {
        0xFF000025,
        0x00FFFFFF,
};

const uint32_t at25ql641_ucode_wakeup_32bit_addressing[] = {
        0xFF000045,
        0xFFFFFFFF,
        0x00FFFFFF,
};

#endif

/********************************************** FUNCTION PROTOTYPES **********************************************/
/**
 * \brief Set the Quad Enable Bit of the Status Register if it is not already set.
 *
 * \param[in] id QSPI controller id
 */
__RETAINED_CODE static void flash_at25ql641_enable_quad_mode(HW_QSPIC_ID id);

/**
 * \brief This function returns the dummy bytes required when utilizing Fast Read Quad.
 *
 * \param[in] id QSPI controller id
 *
 * \note This function will return the number of dummy bytes required while Fast Read Quad is activated. In some QSPI Flash devices,
 *       it is possible that the number of dummy bytes required can vary depending on the operating speed.
 */
__RETAINED_CODE static uint8_t flash_at25ql641_get_dummy_bytes(HW_QSPIC_ID id);

/**
 * \brief Initialize AT25QL641 QSPI Flash
 *
 * \param[in] id QSPI controller id
 *
 * \note This function will perform all the required actions in order for AT25QL641 QSPI Flash to
 *       be properly initialized.
 */
__RETAINED_CODE static void flash_at25ql641_initialize(HW_QSPIC_ID id);

/**
 * \brief This function returns true if an erase or program operation is already suspended.
 *
 * \param[in] id QSPI controller id
 *
 * \note This function reads the status register and checks the erase/program suspend bit. If it is set then it return true since
 *       a program or erase operation is currently suspended.
 */
__RETAINED_CODE static bool flash_at25ql641_is_suspended(HW_QSPIC_ID id);

/**
 * \brief Read the Status Register 2 of the Flash
 *
 * \param[in] id QSPI controller id
 *
 * \return uint8_t The value of the Status Register 2 of the Flash.
 */
__RETAINED_CODE static uint8_t flash_at25ql641_read_status_register_2(HW_QSPIC_ID id);

/**
 * \brief Hook function called when the clock is changed
 *
 * \param[in] id QSPI controller id
 *
 * \param[in] sys_clk System clock source to be switched to
 *
 * \note This function will be called prior to switching from one clock source to another. This function allows doing all
 *       the required tasks before the clock source is switched. Typical use could be setting up the proper QSPI divider
 *       to ensure that the device will continue to operate even after the clock source will be switched. E.g. Flash that supports
 *       up to 80 MHz operating speed will need to have a divider of 2 if PLL96 is picked as the source.
 */
__RETAINED_CODE static void flash_at25ql641_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk);

/**
 * \brief Write the Status Register 2 of the Flash
 *
 * \param[in] id QSPI controller id
 * \param[in] value The value to be written.
 *
 * \note This function blocks until the Flash has processed the command. No verification that the
 *        value has been actually written is done though. It is up to the caller to decide whether
 *        such verification is needed or not and execute it on its own.
 */
__RETAINED_CODE static void flash_at25ql641_write_status_register_2(HW_QSPIC_ID id, uint8_t value);

/************************************************* CONFIG OBJECT *************************************************/

/**
 * \brief This struct configures the system for the specific flash
 *
 * \note This struct MUST be const for this to work. Therefore, assignments must
 *       not change (must be read-only)
 */
static const qspi_flash_config_t flash_at25ql641_config = {
        /* JEDEC Bytes 9Fh */
        .manufacturer_id                  = ADESTO_ID,
        .device_type                      = AT25QL_TYPE,
        .device_density                   = AT25QL641_SIZE,

        /* Flash Info */
        .memory_size                      = MEMORY_SIZE_32Mb,
        .address_size                     = HW_QSPI_ADDR_SIZE_24,
        .is_ram                           = false,
        .qpi_mode                         = false,

        /* Callbacks */
        .is_suspended                     = flash_at25ql641_is_suspended,
        .initialize                       = flash_at25ql641_initialize,
        .sys_clk_cfg                      = flash_at25ql641_sys_clock_cfg,
        .get_dummy_bytes                  = flash_at25ql641_get_dummy_bytes,

        /* Read */
        .fast_read_opcode                 = AT25QL641_FAST_READ_QUAD_OPCODE,
        .send_once                        = 1,
        .extra_byte                       = AT25QL641_EXTRA_BYTE_M7_M0,
        .break_seq_size                   = HW_QSPI_BREAK_SEQ_SIZE_1B,

        /* Page Program */
        .page_program_opcode              = AT25QL641_QUAD_PAGE_PROGRAM_OPCODE,
        .quad_page_program_address        = true,

        /* Sector Erase */
        .erase_opcode                     = CMD_SECTOR_ERASE,
        .read_erase_progress_opcode       = CMD_READ_STATUS_REGISTER,
        .erase_in_progress_bit            = FLASH_STATUS_BUSY_BIT,
        .erase_in_progress_bit_high_level = true,

        /* Program/Erase Suspend/Resume */
        .erase_suspend_opcode             = AT25QL641_ERASE_PROGRAM_SUSPEND_OPCODE,
        .erase_resume_opcode              = AT25QL641_ERASE_PROGRAM_RESUME_OPCODE,

        /* Timings */
        .power_down_delay                 = AT25QL641_POWER_DOWN_DELAY_US,
        .release_power_down_delay         = AT25QL641_RELEASE_POWER_DOWN_DELAY_US,
        .power_up_delay                   = AT25QL641_POWER_UP_DELAY_US,
};

/********************************************* FUNCTION  DEFINITIONS *********************************************/

__RETAINED_CODE static void flash_at25ql641_enable_quad_mode(HW_QSPIC_ID id)
{
        uint8_t status;

        status = flash_at25ql641_read_status_register_2(id);
        if (!(status & AT25QL641_SR_2_QE_MASK)) {
                flash_write_enable(id);
                flash_at25ql641_write_status_register_2(id, status | AT25QL641_SR_2_QE_MASK);
        }
}

__RETAINED_CODE static uint8_t flash_at25ql641_get_dummy_bytes(HW_QSPIC_ID id)
{
        /* 2 Dummy Bytes are required for non QPI mode of operation regardless of Operating Frequency for AT25QL641. */
        return 2;
}

__RETAINED_CODE static void flash_at25ql641_initialize(HW_QSPIC_ID id)
{
        /* Set QE Bit if it is not set */
        flash_at25ql641_enable_quad_mode(id);
}

__RETAINED_CODE static bool flash_at25ql641_is_suspended(HW_QSPIC_ID id)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;

        status = flash_at25ql641_read_status_register_2(id);
        return ((status & AT25QL641_SR_2_ESUS_MASK) != 0);
}

__RETAINED_CODE static uint8_t flash_at25ql641_read_status_register_2(HW_QSPIC_ID id)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t cmd[] = { AT25QL641_READ_STATUS_REGISTER_2_OPCODE };

        flash_transact(id, cmd, 1, &status, 1);

        return status;
}

__RETAINED_CODE static void flash_at25ql641_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk)
{
        /* Operating Frequency larger than 96 MHz achieved through PLL so there is no need to use a divider. Empty hook function */
}

__RETAINED_CODE static void flash_at25ql641_write_status_register_2(HW_QSPIC_ID id, uint8_t value)
{
        uint8_t cmd[2] = { AT25QL641_WRITE_STATUS_REGISTER_2_OPCODE, value };

        flash_write(id, cmd, 2);

        /* Wait for the Flash to process the command */
        while (flash_is_busy(id));
}

#endif /* _QSPI_ADESTO_AT25QL641_H_ */
/**
 * \}
 * \}
 */

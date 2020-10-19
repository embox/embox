/*
 * custom_config_qspi.h
 *
 *  Created on: May 7, 2020
 *      Author: anton
 */

#ifndef THIRD_PARTY_BSP_DIALOG_CUSTOM_CONFIG_QSPI_H_
#define THIRD_PARTY_BSP_DIALOG_CUSTOM_CONFIG_QSPI_H_

#include "bsp_definitions.h"

//FIXME
#define OS_DELAY_MS(delay)

#define USE_RTOS          1

#define CONFIG_USE_BLE

/*************************************************************************************************\
 * System configuration
 */
#define dg_configDEFAULT_CLK					( sysclk_XTAL32M )
#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configEXEC_MODE                      ( MODE_IS_CACHED )
#define dg_configCODE_LOCATION                  ( NON_VOLATILE_IS_FLASH )

//#define CONFIG_RETARGET							( 1 )
#define CONFIG_RETARGET_UART_BAUDRATE    		( HW_UART_BAUDRATE_1000000 )

#if 0
#define dg_configUSE_WDOG                       ( 1 )
#define dg_configWDOG_MAX_TASKS_CNT				( 16 )
#else
#define dg_configUSE_WDOG                       ( 0 )
#define dg_configWDOG_MAX_TASKS_CNT				( 0 )
#endif

#define dg_configFLASH_CONNECTED_TO             ( FLASH_CONNECTED_TO_1V8 )
#define dg_configFLASH_POWER_DOWN               ( 1 )
#define dg_configPOWER_1V8_ACTIVE               ( 1 )
#define dg_configPOWER_1V8_SLEEP                ( 1 )
#define dg_configPOWER_1V8P                     ( 1 )

//USB
#define dg_configUSE_USB                        ( 1 )
#define dg_configUSB_SUSPEND_MODE               ( USB_SUSPEND_MODE_NONE )
#define dg_configUSE_USB_CHARGER                ( 1 )
#define dg_configUSE_USB_ENUMERATION            ( 1 )
#define dg_configALLOW_CHARGING_NOT_ENUM        ( 1 )
#define dg_configUSE_NOT_ENUM_CHARGING_TIMEOUT  ( 0 )

//CHARGER
#define dg_configUSE_SYS_CHARGER				( 0	)

#define dg_configUSE_SW_CURSOR                  ( 1 )

//
// Enable the settings below to track OS heap usage, for profiling
//
#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
#define dg_configTRACK_OS_HEAP                  ( 1 )
#else
#define dg_configTRACK_OS_HEAP                  ( 0 )
#endif

#define dg_configCACHEABLE_QSPI_AREA_LEN        ( NVMS_PARAM_PART_start - MEMORY_QSPIF_BASE )


/*************************************************************************************************\
 * Peripherals configuration
 */
#if 0
#define dg_configFLASH_ADAPTER                  ( 1 )
#define dg_configNVMS_ADAPTER                   ( 1 )
#define dg_configNVMS_VES                       ( 1 )
#define dg_configNVPARAM_ADAPTER                ( 1 )
#else
#define dg_configFLASH_ADAPTER                  ( 0 )
#define dg_configNVMS_ADAPTER                   ( 0 )
#define dg_configNVMS_VES                       ( 0 )
#define dg_configNVPARAM_ADAPTER                ( 0 )
#endif

/*************************************************************************************************\
 * BLE configuration
 */
//#define CONFIG_BLE_STORAGE
#if 0
#define CONFIG_USE_BLE_CLIENTS
#define CONFIG_USE_BLE_SERVICES
#define dg_configBLE_CENTRAL                    ( 1 )
#define dg_configBLE_GATT_CLIENT                ( 1 )
#define dg_configBLE_GATT_SERVER                ( 1 )
#define dg_configBLE_OBSERVER                   ( 1 )
#define dg_configBLE_BROADCASTER                ( 1 )
#define dg_configBLE_L2CAP_COC                  ( 1 )
#else
#define dg_configBLE_CENTRAL                    ( 0 )
#define dg_configBLE_GATT_CLIENT                ( 0 )
#define dg_configBLE_GATT_SERVER                ( 0 )
#define dg_configBLE_OBSERVER                   ( 0 )
#define dg_configBLE_BROADCASTER                ( 0 )
#define dg_configBLE_L2CAP_COC                  ( 0 )
// REMOVE
#define USE_BLE_SLEEP                           ( 1 )
#endif

#define dg_configUSE_HW_GPIO                    ( 1 )
#define dg_configUSE_HW_WKUP                    ( 1 )
#define dg_configUSE_HW_DMA						( 1 )
#define dg_configUSE_HW_I2C                    	( 1 )
#define dg_configUSE_HW_SPI                    	( 1 )

#define dg_configUSE_CLOCK_MGR                  ( 1 )

#define dg_configRF_ENABLE_RECALIBRATION        ( 0 )

#define BLE_MGR_USE_EVT_LIST                    ( 0 )

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* THIRD_PARTY_BSP_DIALOG_CUSTOM_CONFIG_QSPI_H_ */

/*
 * custom_config_qspi.h
 *
 *  Created on: May 7, 2020
 *      Author: anton
 */

#ifndef THIRD_PARTY_BSP_DIALOG_CUSTOM_CONFIG_QSPI_H_
#define THIRD_PARTY_BSP_DIALOG_CUSTOM_CONFIG_QSPI_H_

#include "bsp_definitions.h"

#define CONFIG_USE_BLE
#undef CONFIG_USE_FTDF

#define CONFIG_LARGE_RESOURCE_ID						(1)

#ifndef RELEASE_BUILD
#define dg_configIMAGE_SETUP                    		(DEVELOPMENT_MODE)
#else
#define dg_configIMAGE_SETUP                    		(PRODUCTION_MODE)
#endif

//#define dg_configUSE_LP_CLK                     		(LP_CLK_32768)
#define dg_configUSE_LP_CLK                     		(LP_CLK_RCX)

#define dg_configEXEC_MODE                      		(MODE_IS_CACHED)
#define dg_configCODE_LOCATION                  		(NON_VOLATILE_IS_FLASH)
#define dg_configEXT_CRYSTAL_FREQ               		(EXT_CRYSTAL_IS_16M)

#define dg_configEMULATE_OTP_COPY               		(0)

#define dg_configUSER_CAN_USE_TIMER1            		(0)

//#define dg_configOPTIMAL_RETRAM                 		(1)
#define dg_configOPTIMAL_RETRAM                 		(0)

#if (dg_configOPTIMAL_RETRAM == 1)
#if (dg_configBLACK_ORCA_IC_REV == BLACK_ORCA_IC_REV_A)
#define dg_configMEM_RETENTION_MODE             		(0x1B)
#define dg_configSHUFFLING_MODE                 		(0x0)
#else
#define dg_configMEM_RETENTION_MODE             		(0x07)
#define dg_configSHUFFLING_MODE                 		(0x3)
#endif
#endif

//#define dg_configSYSTEMVIEW 							(1)

#define dg_configUSE_WDOG                       		(1)
#define dg_configWDOG_MAX_TASKS_CNT						(16)

#define dg_configFLASH_CONNECTED_TO             		(FLASH_CONNECTED_TO_1V8)
#define dg_configFLASH_POWER_DOWN               		(1)

#define dg_configPOWER_1V8_ACTIVE               		(1)
#define dg_configPOWER_1V8_SLEEP                		(1)
#define dg_configPOWER_1V8P                     		(1)

//USB & CHARGER
#define dg_configUSE_USB                                (1)
#define dg_configUSB_SUSPEND_MODE                       (1)
#define dg_configUSE_USB_CHARGER                        (0)
#define dg_configUSE_USB_ENUMERATION                    (1)
#define dg_configALLOW_CHARGING_NOT_ENUM                (1)
#define dg_configUSE_NOT_ENUM_CHARGING_TIMEOUT          (0)

#if dg_configUSE_USB_CHARGER
#define dg_configBATTERY_TYPE                           (BATTERY_TYPE_CUSTOM)
#define dg_configBATTERY_TYPE_CUSTOM_ADC_VOLTAGE        (3440)            		// 4.2V
#define dg_configBATTERY_CHARGE_CURRENT                 (5)               		// 90 mA - 0.6C (see table 813 in datasheet)
#define dg_configBATTERY_PRECHARGE_CURRENT              (29)              		// 15.3mA - 0.1C
#define dg_configBATTERY_CHARGE_NTC                     (1)               		// disabled
#define dg_configPRECHARGING_THRESHOLD                  (2460)            	    // 3.006V
#define dg_configCHARGING_THRESHOLD                     (2496)		            // 3.05V
#define dg_configPRECHARGING_TIMEOUT                    (360*60*100)	        // 6 hours
#define dg_configCHARGING_CC_TIMEOUT                    (360*60*100)	        // 6 hours
#define dg_configCHARGING_CV_TIMEOUT                    (360*60*100)	        // 6 hours
#define dg_configUSB_CHARGER_POLLING_INTERVAL           (1*60*100)	            // 1 minute
#define dg_configBATTERY_CHARGE_VOLTAGE                 (0x0A)		            // the hardware settings for 4.2V
#define dg_configBATTERY_LOW_LEVEL                      (2537)		            // 3.1V
#else
#define dg_configBATTERY_TYPE                           (BATTERY_TYPE_LIMN2O4)
#define dg_configBATTERY_CHARGE_CURRENT                 (2)       				// 30mA
#define dg_configBATTERY_PRECHARGE_CURRENT              (20)      				// 2.1mA
#define dg_configBATTERY_CHARGE_NTC                     (1)       				// disabled
#endif //dg_configUSE_USB_CHARGER

//BATTERY CONTROL
#define dg_configUSE_SOC                				(0)

#define dg_configUSE_ProDK                      		(1)

#define dg_configUSE_SW_CURSOR                 			(1)

#define dg_configUSE_HW_I2C                     		(1)
#define dg_configUSE_HW_SPI                     		(1)
#define dg_configUSE_HW_USB                     		(1)
#define dg_configUSE_HW_TIMER2							(1)
#define dg_configUSE_HW_AES_HASH						(1)

/*************************************************************************************************\
 * Memory specific config
 */
#define dg_configQSPI_CODE_SIZE                 		(512 * 1024)
#define dg_configQSPI_CACHED_OPTIMAL_RETRAM_0_SIZE_AE   (64 * 1024)
#define dg_configQSPI_CACHED_RAM_SIZE_AE                (56 * 1024)
#define dg_configQSPI_CACHED_RETRAM_0_SIZE_AE           (72 * 1024)
#define dg_configQSPI_CACHED_RETRAM_1_SIZE_AE			(0 * 1024)

/*************************************************************************************************\
 * Peripheral specific config
 */
#define dg_configFLASH_ADAPTER                  		(1)
#define dg_configNVMS_ADAPTER                   		(1)
#define dg_configNVMS_VES                       		(0)
#define dg_configGPADC_ADAPTER                  		(0)
#define dg_configI2C_ADAPTER                    		(0)
#define dg_configSPI_ADAPTER							(0)

#define dg_configCACHEABLE_QSPI_AREA_LEN        		(NVMS_PARAM_PART_start - MEMORY_QSPIF_BASE)

/*************************************************************************************************\
 * BLE device config
 */
#define dg_configBLE_CENTRAL                    		(1)
#define dg_configBLE_GATT_CLIENT                		(1)
#define dg_configBLE_GATT_SERVER                		(1)
#define dg_configBLE_OBSERVER                   		(1)
#define dg_configBLE_BROADCASTER                		(1)
#define dg_configBLE_L2CAP_COC                  		(1)

//#define dg_configUSE_HW_CPM                             (1)

/* Include bsp default values */
#include "bsp_defaults.h"

/* Include memory layout */
#include "bsp_memory_layout.h"

#endif /* THIRD_PARTY_BSP_DIALOG_CUSTOM_CONFIG_QSPI_H_ */

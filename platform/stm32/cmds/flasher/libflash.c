/**
 * @file
 *
 * @date   30.10.2020
 * @author Alexander Kalmuk
 */

#include <stdint.h>

#include "stm32_sys_helper.h"


/**
  * @brief FLASH Registers
  */
#if 0
typedef struct
{
  __IO uint32_t ACR;      /*!< FLASH access control register,     Address offset: 0x00 */
  __IO uint32_t KEYR;     /*!< FLASH key register,                Address offset: 0x04 */
  __IO uint32_t OPTKEYR;  /*!< FLASH option key register,         Address offset: 0x08 */
  __IO uint32_t SR;       /*!< FLASH status register,             Address offset: 0x0C */
  __IO uint32_t CR;       /*!< FLASH control register,            Address offset: 0x10 */
  __IO uint32_t OPTCR;    /*!< FLASH option control register ,    Address offset: 0x14 */
  __IO uint32_t OPTCR1;   /*!< FLASH option control register 1 ,  Address offset: 0x18 */
} FLASH_TypeDef;
#endif

#define PERIPH_BASE            0x40000000UL /*!< Base address of : AHB/ABP Peripherals */

#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000UL)

#define FLASH_R_BASE          (AHB1PERIPH_BASE + 0x3C00UL)

struct flash_ctrl {
  volatile uint32_t ACR;      /*!< FLASH access control register,     Address offset: 0x00 */
  volatile uint32_t KEYR;     /*!< FLASH key register,                Address offset: 0x04 */
  volatile uint32_t OPTKEYR;  /*!< FLASH option key register,         Address offset: 0x08 */
  volatile uint32_t SR;       /*!< FLASH status register,             Address offset: 0x0C */
  volatile uint32_t CR;       /*!< FLASH control register,            Address offset: 0x10 */
  volatile uint32_t OPTCR;    /*!< FLASH option control register ,    Address offset: 0x14 */
  volatile uint32_t OPTCR1;   /*!< FLASH option control register 1 ,  Address offset: 0x18 */
};

#define FLASH    ((struct flash_ctrl *volatile)FLASH_R_BASE)

#define FLASH_SECTOR_11              11

#define FLASH_VOLTAGE_RANGE_1        ((uint32_t)0x00U)  /*!< Device operating range: 1.8V to 2.1V                */
#define FLASH_VOLTAGE_RANGE_2        ((uint32_t)0x01U)  /*!< Device operating range: 2.1V to 2.7V                */
#define FLASH_VOLTAGE_RANGE_3        ((uint32_t)0x02U)  /*!< Device operating range: 2.7V to 3.6V                */
#define FLASH_VOLTAGE_RANGE_4        ((uint32_t)0x03U)  /*!< Device operating range: 2.7V to 3.6V + External Vpp */

/** @defgroup FLASH_Program_Parallelism FLASH Program Parallelism
  * @{
  */
#define FLASH_PSIZE_BYTE           ((uint32_t)0x00000000U)
#define FLASH_PSIZE_HALF_WORD      ((uint32_t)FLASH_CR_PSIZE_0)
#define FLASH_PSIZE_WORD           ((uint32_t)FLASH_CR_PSIZE_1)
#define FLASH_PSIZE_DOUBLE_WORD    ((uint32_t)FLASH_CR_PSIZE)
#define CR_PSIZE_MASK              ((uint32_t)0xFFFFFCFFU)

/** @defgroup FLASH_Keys FLASH Keys
  * @{
  */
#define FLASH_KEY1               ((uint32_t)0x45670123U)
#define FLASH_KEY2               ((uint32_t)0xCDEF89ABU)
#define FLASH_OPT_KEY1           ((uint32_t)0x08192A3BU)
#define FLASH_OPT_KEY2           ((uint32_t)0x4C5D6E7FU)

/*******************  Bits definition for FLASH_ACR register  *****************/
#define FLASH_ACR_LATENCY_Pos         (0U)
#define FLASH_ACR_LATENCY_Msk         (0xFUL << FLASH_ACR_LATENCY_Pos)          /*!< 0x0000000F */
#define FLASH_ACR_LATENCY             FLASH_ACR_LATENCY_Msk
#define FLASH_ACR_LATENCY_0WS         0x00000000U
#define FLASH_ACR_LATENCY_1WS         0x00000001U
#define FLASH_ACR_LATENCY_2WS         0x00000002U
#define FLASH_ACR_LATENCY_3WS         0x00000003U
#define FLASH_ACR_LATENCY_4WS         0x00000004U
#define FLASH_ACR_LATENCY_5WS         0x00000005U
#define FLASH_ACR_LATENCY_6WS         0x00000006U
#define FLASH_ACR_LATENCY_7WS         0x00000007U
#define FLASH_ACR_LATENCY_8WS         0x00000008U
#define FLASH_ACR_LATENCY_9WS         0x00000009U
#define FLASH_ACR_LATENCY_10WS        0x0000000AU
#define FLASH_ACR_LATENCY_11WS        0x0000000BU
#define FLASH_ACR_LATENCY_12WS        0x0000000CU
#define FLASH_ACR_LATENCY_13WS        0x0000000DU
#define FLASH_ACR_LATENCY_14WS        0x0000000EU
#define FLASH_ACR_LATENCY_15WS        0x0000000FU
#define FLASH_ACR_PRFTEN_Pos          (8U)
#define FLASH_ACR_PRFTEN_Msk          (0x1UL << FLASH_ACR_PRFTEN_Pos)           /*!< 0x00000100 */
#define FLASH_ACR_PRFTEN              FLASH_ACR_PRFTEN_Msk
#define FLASH_ACR_ARTEN_Pos           (9U)
#define FLASH_ACR_ARTEN_Msk           (0x1UL << FLASH_ACR_ARTEN_Pos)            /*!< 0x00000200 */
#define FLASH_ACR_ARTEN               FLASH_ACR_ARTEN_Msk
#define FLASH_ACR_ARTRST_Pos          (11U)
#define FLASH_ACR_ARTRST_Msk          (0x1UL << FLASH_ACR_ARTRST_Pos)           /*!< 0x00000800 */
#define FLASH_ACR_ARTRST              FLASH_ACR_ARTRST_Msk

/*******************  Bits definition for FLASH_SR register  ******************/
#define FLASH_SR_EOP_Pos              (0U)
#define FLASH_SR_EOP_Msk              (0x1UL << FLASH_SR_EOP_Pos)               /*!< 0x00000001 */
#define FLASH_SR_EOP                  FLASH_SR_EOP_Msk
#define FLASH_SR_OPERR_Pos            (1U)
#define FLASH_SR_OPERR_Msk            (0x1UL << FLASH_SR_OPERR_Pos)             /*!< 0x00000002 */
#define FLASH_SR_OPERR                FLASH_SR_OPERR_Msk
#define FLASH_SR_WRPERR_Pos           (4U)
#define FLASH_SR_WRPERR_Msk           (0x1UL << FLASH_SR_WRPERR_Pos)            /*!< 0x00000010 */
#define FLASH_SR_WRPERR               FLASH_SR_WRPERR_Msk
#define FLASH_SR_PGAERR_Pos           (5U)
#define FLASH_SR_PGAERR_Msk           (0x1UL << FLASH_SR_PGAERR_Pos)            /*!< 0x00000020 */
#define FLASH_SR_PGAERR               FLASH_SR_PGAERR_Msk
#define FLASH_SR_PGPERR_Pos           (6U)
#define FLASH_SR_PGPERR_Msk           (0x1UL << FLASH_SR_PGPERR_Pos)            /*!< 0x00000040 */
#define FLASH_SR_PGPERR               FLASH_SR_PGPERR_Msk
#define FLASH_SR_ERSERR_Pos           (7U)
#define FLASH_SR_ERSERR_Msk           (0x1UL << FLASH_SR_ERSERR_Pos)            /*!< 0x00000080 */
#define FLASH_SR_ERSERR               FLASH_SR_ERSERR_Msk
#define FLASH_SR_BSY_Pos              (16U)
#define FLASH_SR_BSY_Msk              (0x1UL << FLASH_SR_BSY_Pos)               /*!< 0x00010000 */
#define FLASH_SR_BSY                  FLASH_SR_BSY_Msk


/** @defgroup FLASH_Flag_definition FLASH Flag definition
  * @brief Flag definition
  * @{
  */
#define FLASH_FLAG_EOP                 FLASH_SR_EOP            /*!< FLASH End of Operation flag               */
#define FLASH_FLAG_OPERR               FLASH_SR_OPERR          /*!< FLASH operation Error flag                */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPERR         /*!< FLASH Write protected error flag          */
#define FLASH_FLAG_PGAERR              FLASH_SR_PGAERR         /*!< FLASH Programming Alignment error flag    */
#define FLASH_FLAG_PGPERR              FLASH_SR_PGPERR         /*!< FLASH Programming Parallelism error flag  */
#define FLASH_FLAG_ERSERR              FLASH_SR_ERSERR         /*!< FLASH Erasing Sequence error flag         */
#define FLASH_FLAG_BSY                 FLASH_SR_BSY            /*!< FLASH Busy flag                           */

#if defined (FLASH_OPTCR2_PCROP)
#define FLASH_FLAG_RDERR               FLASH_SR_RDERR          /*!< FLASH Read protection error flag          */
#define FLASH_FLAG_ALL_ERRORS     (FLASH_FLAG_OPERR   | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                                   FLASH_FLAG_PGPERR  | FLASH_FLAG_ERSERR | FLASH_FLAG_RDERR)
#else
#define FLASH_FLAG_ALL_ERRORS     (FLASH_FLAG_OPERR   | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                                   FLASH_FLAG_PGPERR  | FLASH_FLAG_ERSERR)
#endif /* FLASH_OPTCR2_PCROP */


/*******************  Bits definition for FLASH_CR register  ******************/
#define FLASH_CR_PG_Pos               (0U)
#define FLASH_CR_PG_Msk               (0x1UL << FLASH_CR_PG_Pos)                /*!< 0x00000001 */
#define FLASH_CR_PG                   FLASH_CR_PG_Msk
#define FLASH_CR_SER_Pos              (1U)
#define FLASH_CR_SER_Msk              (0x1UL << FLASH_CR_SER_Pos)               /*!< 0x00000002 */
#define FLASH_CR_SER                  FLASH_CR_SER_Msk
#define FLASH_CR_MER_Pos              (2U)
#define FLASH_CR_MER_Msk              (0x1UL << FLASH_CR_MER_Pos)               /*!< 0x00000004 */
#define FLASH_CR_MER                  FLASH_CR_MER_Msk
#define FLASH_CR_MER1                        FLASH_CR_MER
#define FLASH_CR_SNB_Pos              (3U)
#define FLASH_CR_SNB_Msk              (0x1FUL << FLASH_CR_SNB_Pos)              /*!< 0x000000F8 */
#define FLASH_CR_SNB                  FLASH_CR_SNB_Msk
#define FLASH_CR_SNB_0                0x00000008U
#define FLASH_CR_SNB_1                0x00000010U
#define FLASH_CR_SNB_2                0x00000020U
#define FLASH_CR_SNB_3                0x00000040U
#define FLASH_CR_SNB_4                0x00000080U
#define FLASH_CR_PSIZE_Pos            (8U)
#define FLASH_CR_PSIZE_Msk            (0x3UL << FLASH_CR_PSIZE_Pos)             /*!< 0x00000300 */
#define FLASH_CR_PSIZE                FLASH_CR_PSIZE_Msk
#define FLASH_CR_PSIZE_0              (0x1UL << FLASH_CR_PSIZE_Pos)             /*!< 0x00000100 */
#define FLASH_CR_PSIZE_1              (0x2UL << FLASH_CR_PSIZE_Pos)             /*!< 0x00000200 */
#define FLASH_CR_MER2_Pos             (15U)
#define FLASH_CR_MER2_Msk             (0x1UL << FLASH_CR_MER2_Pos)              /*!< 0x00008000 */
#define FLASH_CR_MER2                 FLASH_CR_MER2_Msk
#define FLASH_CR_STRT_Pos             (16U)
#define FLASH_CR_STRT_Msk             (0x1UL << FLASH_CR_STRT_Pos)              /*!< 0x00010000 */
#define FLASH_CR_STRT                 FLASH_CR_STRT_Msk
#define FLASH_CR_EOPIE_Pos            (24U)
#define FLASH_CR_EOPIE_Msk            (0x1UL << FLASH_CR_EOPIE_Pos)             /*!< 0x01000000 */
#define FLASH_CR_EOPIE                FLASH_CR_EOPIE_Msk
#define FLASH_CR_ERRIE_Pos            (25U)
#define FLASH_CR_ERRIE_Msk            (0x1UL << FLASH_CR_ERRIE_Pos)             /*!< 0x02000000 */
#define FLASH_CR_ERRIE                FLASH_CR_ERRIE_Msk
#define FLASH_CR_LOCK_Pos             (31U)
#define FLASH_CR_LOCK_Msk             (0x1UL << FLASH_CR_LOCK_Pos)              /*!< 0x80000000 */
#define FLASH_CR_LOCK                 FLASH_CR_LOCK_Msk

/**
  * @brief  Unlock the FLASH control register access
  * @retval HAL Status
  */
int libflash_flash_unlock(void) {
  int status = 0;

	if (FLASH->CR & FLASH_CR_LOCK) {
    /* Authorize the FLASH Registers access */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;

    /* Verify Flash is unlocked */
		if (FLASH->CR & FLASH_CR_LOCK) {
			status = -1;
		}
	}

	return status;
}

/**
  * @brief  Locks the FLASH control register access
  * @retval HAL Status
  */
int libflash_flash_lock(void) {
  /* Set the LOCK Bit to lock the FLASH Registers access */
  FLASH->CR |= FLASH_CR_LOCK;
  
  return 0;  
}

/**
  * @brief  Wait for a FLASH operation to complete.
  * @param  Timeout maximum flash operationtimeout
  * @retval HAL Status
  */
static inline int libflash_flash_wait_for_op(void) {
	while ( (FLASH->SR & FLASH_FLAG_BSY) != 0){
	}

	if ( (FLASH->SR & FLASH_FLAG_ALL_ERRORS) != 0) {
			return -1;
	}

	/* Check FLASH End of Operation flag  */
	if ( (FLASH->SR & FLASH_FLAG_EOP) != 0) {
			/* Clear FLASH End of Operation pending bit */
			FLASH->SR &= ~FLASH_FLAG_EOP;
	}

	/* If there is an error flag set */
	return 0;
}

/**
  * @brief  Program a double word (64-bit) at a specified address.
  * @note   This function must be used when the device voltage range is from
  *         2.7V to 3.6V and an External Vpp is present.
  *
  * @note   If an erase and a program operations are requested simultaneously,    
  *         the erase operation is performed before the program one.
  *  
  * @param  Address specifies the address to be programmed.
  * @param  Data specifies the data to be programmed.
  * @retval None
  */
void libflash_program_64(uint32_t add, uint64_t data) {
  
  /* If the previous operation is completed, proceed to program the new data */
  FLASH->CR &= CR_PSIZE_MASK;
  FLASH->CR |= FLASH_PSIZE_DOUBLE_WORD;
  FLASH->CR |= FLASH_CR_PG;

  /* Program first word */
  *(volatile uint32_t*)add = (uint32_t)data;
  /* Barrier to ensure programming is performed in 2 steps, in right order
    (independently of compiler optimization behavior) */
  __ISB();

  /* Program second word */
  *(volatile uint32_t*)(add + 4) = (uint32_t)(data >> 32);

  /* Data synchronous Barrier (DSB) Just after the write operation
     This will force the CPU to respect the sequence of instruction (no optimization).*/
  __DSB();
}



/**
  * @brief  Erase the specified FLASH memory sector
  * @param  Sector FLASH sector to erase
  *         The value of this parameter depend on device used within the same series      
  * 
  * @retval None
  */
void libflash_erase_sector(uint32_t Sector) {
  uint8_t voltage = FLASH_VOLTAGE_RANGE_3; 
	uint32_t tmp_psize = 0;

	if (voltage == FLASH_VOLTAGE_RANGE_1) {
		tmp_psize = FLASH_PSIZE_BYTE;
	}
	else if (voltage == FLASH_VOLTAGE_RANGE_2) {
		tmp_psize = FLASH_PSIZE_HALF_WORD;
	}
	else if (voltage == FLASH_VOLTAGE_RANGE_3) {
		tmp_psize = FLASH_PSIZE_WORD;
	}
	else {
		tmp_psize = FLASH_PSIZE_DOUBLE_WORD;
	}

	/* Need to add offset of 4 when sector higher than FLASH_SECTOR_11 */
	if (Sector > FLASH_SECTOR_11) {
		Sector += 4;
	}

	/* If the previous operation is completed, proceed to erase the sector */
	FLASH->CR &= CR_PSIZE_MASK;
	FLASH->CR |= tmp_psize;
	FLASH->CR &= ~FLASH_CR_SNB;
	FLASH->CR |= FLASH_CR_SER | (Sector << FLASH_CR_SNB_Pos);
	FLASH->CR |= FLASH_CR_STRT;

	/* Data synchronous Barrier (DSB) Just after the write operation
     This will force the CPU to respect the sequence of instruction
     (no optimization).*/
	__DSB();


  /* Wait for last operation to be completed */
  libflash_flash_wait_for_op();

  /* If the erase operation is completed, disable the SER Bit and SNB Bits */
  FLASH->CR &= ~(FLASH_CR_SER | FLASH_CR_SNB); 
}

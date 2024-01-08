/**
 * @file
 *
 * @date   30.10.2020
 * @author Alexander Kalmuk
 */

#include <stdint.h>

#ifndef   __STATIC_FORCEINLINE             
  #define __STATIC_FORCEINLINE                   \
              __attribute__((always_inline)) static inline
#endif

/**
  \brief   Instruction Synchronization Barrier
  \details Instruction Synchronization Barrier flushes the pipeline in the processor,
           so that all instructions following the ISB are fetched from cache or memory,
           after the instruction has been completed.
 */
__STATIC_FORCEINLINE void __ISB(void)
{
  __asm volatile ("isb 0xF":::"memory");
}


/**
  \brief   Data Synchronization Barrier
  \details Acts as a special kind of Data Memory Barrier.
           It completes when all explicit memory accesses before this instruction complete.
 */
__STATIC_FORCEINLINE void __DSB(void)
{
  __asm volatile ("dsb 0xF":::"memory");
}


/**
  \brief   Data Memory Barrier
  \details Ensures the apparent order of the explicit memory operations before
           and after the instruction, without ensuring their completion.
 */
__STATIC_FORCEINLINE void __DMB(void)
{
  __asm volatile ("dmb 0xF":::"memory");
}


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
#if 0
/**
  * @brief  Wait for a FLASH operation to complete.
  * @param  Timeout maximum flash operationtimeout
  * @retval HAL Status
  */
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout)
{ 
  uint32_t tickstart = 0;
  
  /* Clear Error Code */
  pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;
  
  /* Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
     Even if the FLASH operation fails, the BUSY flag will be reset and an error
     flag will be set */
  /* Get tick */
  tickstart = HAL_GetTick();

  while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) != RESET) 
  { 
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
      {
        return HAL_TIMEOUT;
      }
    } 
  }
  
  if(__HAL_FLASH_GET_FLAG(FLASH_FLAG_ALL_ERRORS) != RESET)
  {
    /*Save the error code*/
    FLASH_SetErrorCode();
    return HAL_ERROR;
  }
  
  /* Check FLASH End of Operation flag  */
  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP) != RESET)
  {
    /* Clear FLASH End of Operation pending bit */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
  }

  /* If there is an error flag set */
  return HAL_OK;
  
}
#endif
#if 0
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
static void FLASH_Program_DoubleWord(uint32_t Address, uint64_t Data)
{
  /* Check the parameters */
  assert_param(IS_FLASH_ADDRESS(Address));
  
  /* If the previous operation is completed, proceed to program the new data */
  FLASH->CR &= CR_PSIZE_MASK;
  FLASH->CR |= FLASH_PSIZE_DOUBLE_WORD;
  FLASH->CR |= FLASH_CR_PG;

  /* Program first word */
  *(__IO uint32_t*)Address = (uint32_t)Data;
  /* Barrier to ensure programming is performed in 2 steps, in right order
    (independently of compiler optimization behavior) */
  __ISB();

  /* Program second word */
  *(__IO uint32_t*)(Address+4) = (uint32_t)(Data >> 32);

  /* Data synchronous Barrier (DSB) Just after the write operation
     This will force the CPU to respect the sequence of instruction (no optimization).*/
  __DSB();
}
#endif


/**
  * @brief  Erase the specified FLASH memory sector
  * @param  Sector FLASH sector to erase
  *         The value of this parameter depend on device used within the same series      
  * @param  voltage The device voltage range which defines the erase parallelism.  
  *          This parameter can be one of the following values:
  *            @arg FLASH_VOLTAGE_RANGE_1: when the device voltage range is 1.8V to 2.1V, 
  *                                  the operation will be done by byte (8-bit) 
  *            @arg FLASH_VOLTAGE_RANGE_2: when the device voltage range is 2.1V to 2.7V,
  *                                  the operation will be done by half word (16-bit)
  *            @arg FLASH_VOLTAGE_RANGE_3: when the device voltage range is 2.7V to 3.6V,
  *                                  the operation will be done by word (32-bit)
  *            @arg FLASH_VOLTAGE_RANGE_4: when the device voltage range is 2.7V to 3.6V + External Vpp, 
  *                                  the operation will be done by double word (64-bit)
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
}

#if 0
/**
  * @brief  Perform a mass erase or erase the specified FLASH memory sectors 
  * @param[in]  pEraseInit pointer to an FLASH_EraseInitTypeDef structure that
  *         contains the configuration information for the erasing.
  * 
  * @param[out]  SectorError pointer to variable  that
  *         contains the configuration information on faulty sector in case of error 
  *         (0xFFFFFFFF means that all the sectors have been correctly erased)
  * 
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError)
{
  HAL_StatusTypeDef status = HAL_ERROR;
  uint32_t index = 0;
  
  /* Process Locked */
  __HAL_LOCK(&pFlash);

  /* Check the parameters */
  assert_param(IS_FLASH_TYPEERASE(pEraseInit->TypeErase));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  {
    /*Initialization of SectorError variable*/
    *SectorError = 0xFFFFFFFFU;
    
    if(pEraseInit->TypeErase == FLASH_TYPEERASE_MASSERASE)
    {
      /*Mass erase to be done*/
#if defined (FLASH_OPTCR_nDBANK)      
      FLASH_MassErase((uint8_t) pEraseInit->VoltageRange, pEraseInit->Banks);
#else
      FLASH_MassErase((uint8_t) pEraseInit->VoltageRange);      
#endif /* FLASH_OPTCR_nDBANK */
                      
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
      
      /* if the erase operation is completed, disable the MER Bit */
      FLASH->CR &= (~FLASH_MER_BIT);
    }
    else
    {
      /* Check the parameters */
      assert_param(IS_FLASH_NBSECTORS(pEraseInit->NbSectors + pEraseInit->Sector));

      /* Erase by sector by sector to be done*/
      for(index = pEraseInit->Sector; index < (pEraseInit->NbSectors + pEraseInit->Sector); index++)
      {
        FLASH_Erase_Sector(index, (uint8_t) pEraseInit->VoltageRange);

        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
        
        /* If the erase operation is completed, disable the SER Bit and SNB Bits */
        CLEAR_BIT(FLASH->CR, (FLASH_CR_SER | FLASH_CR_SNB)); 

        if(status != HAL_OK) 
        {
          /* In case of error, stop erase procedure and return the faulty sector*/
          *SectorError = index;
          break;
        }
      }
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&pFlash);

  return status;
}

#endif
/**
 * *****************************************************************************
 *  @file       bmcu_def.h
 *  @author     Baikal electronics SDK team
 *  @brief      SDK common definitions
 *  @version    1.3
 *  @date       2025-06-30
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#ifndef __BMCU_DEF_H
#define __BMCU_DEF_H

#include <assert.h>
#include <stddef.h>

#define MAX_DELAY                       0xFFFFFFFFUL

#define IS_BIT_SET(REG, BIT)            (((REG) & (BIT)) != 0U)
#define IS_BIT_CLR(REG, BIT)            (((REG) & (BIT)) == 0U)

#define UNUSED(X)                       (void)X /* To avoid gcc/g++ warnings */

typedef enum 
{
    RESET = 0,
    SET
} FlagStatus, ITStatus;

#define IS_FLAG_STATUS(__VALUE__)       (((__VALUE__) == RESET) || ((__VALUE__) == SET))
#define IS_IT_STATUS(__VALUE__)         (((__VALUE__) == RESET) || ((__VALUE__) == SET))

typedef enum 
{
    DISABLE = 0, 
    ENABLE
} FunctionalState;

#define IS_FUNCTIONAL_STATE(__VALUE__)  (((__VALUE__) == DISABLE) || ((__VALUE__) == ENABLE))

typedef enum
{
    SUCCESS = 0,
    ERROR
} ErrorStatus;

#define IS_ERROR_STATUS(__VALUE__)      (((__VALUE__) == RESET) || ((__VALUE__) == SET))

#endif /* __BMCU_DEF_H */

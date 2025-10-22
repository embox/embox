/**
 * *****************************************************************************
 *  @file       basis_def.h
 *  @author     Baikal electronics SDK team
 *  @brief      BASIS common register- and bit-oriented definitions
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

#ifndef BASIS_DEF_H
#define BASIS_DEF_H

#include "basis_compiler.h"

/* IO definitions (access restrictions to peripheral registers) */
#ifdef __cplusplus
#define __I             volatile        /*!< Defines 'read only' permissions */
#else
#define __I             volatile const  /*!< Defines 'read only' permissions */
#endif
#define __O             volatile        /*!< Defines 'write only' permissions */
#define __IO            volatile        /*!< Defines 'read/write' permissions */

/* Structure members permissions */
#define __IM            volatile const  /*! Defines 'read only' structure member permissions */
#define __OM            volatile        /*! Defines 'write only' structure member permissions */
#define __IOM           volatile        /*! Defines 'read / write' structure member permissions */

/* Register bitwise operations */
#define SET_BIT(REG, BIT)                       ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)                     ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)                      ((REG) & (BIT))
#define WRITE_REG(REG, VAL)                     ((REG) = (VAL))
#define CLEAR_REG(REG)                          ((REG) = (0x0))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)     WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
#define READ_REG(REG)                           ((REG))
#define POSITION_VAL(VAL)                       (__CTZ(VAL))

/* Stringify macro */
#define __STR(s)        #s
#define STRINGIFY(s)    __STR(s)

#endif /* BASIS_DEF_H */

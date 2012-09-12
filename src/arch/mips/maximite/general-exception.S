/*********************************************************************
 *
 *                  General Exception
 *
 *********************************************************************
 * Filename:        general-exception.S
 *
 * Processor:       PIC32
 *
 * Compiler:        MPLAB C Compiler for PIC32 MCUs
 *                  MPLAB IDE
 * Company:         Microchip Technology Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the 'Company') for its PIC32/PIC24F Microcontroller is intended
 * and supplied to you, the Company's customer, for use solely and
 * exclusively on Microchip PIC32/PIC24F Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ********************************************************************/

#include <p32xxxx.h>
#ifdef __LIBBUILD__
   .file 1 "general-exception.S"
   .loc 1 0
#endif
  ###################
  # Default general exception handler
  ###################
  .section .text.general_exception,"ax",@progbits
  .set noreorder
  .set noat
  .globl _general_exception_context
  .ent _general_exception_context

_general_exception_context:

  # Save off the non-callee saved registers that may get mucked with
  addiu sp, sp, -88
  sw $1,  4(sp)
  sw v0,  8(sp)
  sw v1, 12(sp)
  sw a0, 16(sp)
  sw a1, 20(sp)
  sw a2, 24(sp)
  sw a3, 28(sp)
  sw t0, 32(sp)
  sw t1, 36(sp)
  sw t2, 40(sp)
  sw t3, 44(sp)
  sw t4, 48(sp)
  sw t5, 52(sp)
  sw t6, 56(sp)
  sw t7, 60(sp)
  sw t8, 64(sp)
  sw t9, 68(sp)
  sw ra, 72(sp)
  mflo t0
  sw t0, 76(sp)
  mfhi t0
  sw t0, 80(sp)

  # Pass Cause and Status to the handler function
  mfc0 a0, _CP0_CAUSE
  mfc0 a1, _CP0_STATUS
  jal _general_exception_handler
  nop

  lw t0, 80(sp)
  mthi t0
  lw t0, 76(sp)
  mtlo t0

  lw $1,  4(sp)
  lw v0,  8(sp)
  lw v1, 12(sp)
  lw a0, 16(sp)
  lw a1, 20(sp)
  lw a2, 24(sp)
  lw a3, 28(sp)
  lw t0, 32(sp)
  lw t1, 36(sp)
  lw t2, 40(sp)
  lw t3, 44(sp)
  lw t4, 48(sp)
  lw t5, 52(sp)
  lw t6, 56(sp)
  lw t7, 60(sp)
  lw t8, 64(sp)
  lw t9, 68(sp)
  lw ra, 72(sp)
  addiu sp, sp, 88

  ehb  # do we need this? Execution hazard barriers?
  eret

  .end _general_exception_context


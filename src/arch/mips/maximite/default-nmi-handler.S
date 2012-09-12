/*********************************************************************
 *
 *              Default NMI Handler Implementation
 *
 *********************************************************************
 * Filename:        default-nmi-handler.S
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
	.file 1 "Microchip PIC32 Library"
	.loc 1 0
#endif
	.text
	.align	2
	.weak	_nmi_handler
	.set	nomips16
	.ent	_nmi_handler
_nmi_handler:
	.frame	sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder

        mfc0    k0, _CP0_STATUS                   # retrieve STATUS
        lui     k1, ~(_CP0_STATUS_BEV_MASK >> 16) & 0xffff
        ori     k1, k1, ~_CP0_STATUS_BEV_MASK & 0xffff
        and     k0, k0, k1                        # Clear BEV
        mtc0    k0, _CP0_STATUS                   # store STATUS
        eret

	.set	macro
	.set	reorder
	.end	_nmi_handler
	.size	_nmi_handler, .-_nmi_handler

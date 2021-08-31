/*
 * @file
 *
 * @date Jan 2, 2013
 * @author: Anton Bondarev
 */

#ifndef CIRRUS_LOGIC_H_
#define CIRRUS_LOGIC_H_


#define CL_CRT1B   0x1B /* Extended Display Controls */


#define CL_SEQR7   0x7  /* Extended Sequencer Mode */

#define CL_SEQRB        0xb     /* VCLK0 Numerator */
#define CL_SEQRC        0xc     /* VCLK1 Numerator */
#define CL_SEQRD        0xd     /* VCLK2 Numerator */
#define CL_SEQRE        0xe     /* VCLK3 Numerator */

#define CL_SEQR10  0x10 /* Graphics Cursor X Position */
#define CL_SEQR11  0x11 /* Graphics Cursor Y Position */
#define CL_SEQR12  0x12 /* Graphics Cursor Attribute */
#define CL_SEQR13  0x13 /* Graphics Cursor Pattern Address Offset */

#define CL_SEQR1E       0x1e    /* VCLK3 Denominator and Post-Scalar Value */
#define CL_SEQR1F       0x1f    /* BIOS ROM write enable and MCLK Select */


#define CL_GRB     0xB  /* GRB: Graphics Controller Mode Extensions */
#define CL_GRC     0xC  /* Color Key/Chroma Key Compare */
#define CL_GRD     0xD  /* Color Key/Mask/Chroma Key */
#define CL_GRE     0xE  /* Power Management */

#define CL_GR31    0x31 /* BLT Start/Status */

#define CL_CRT51   0x51 /* Video Capture Data Format */




#endif /* CIRRUS_LOGIC_H_ */

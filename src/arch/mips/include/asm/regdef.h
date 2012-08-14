/**
 * @file
 *
 * @brief
 *
 * @date 26.07.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_REGDEF_H_
#define MIPS_REGDEF_H_

//http://www.doc.ic.ac.uk/lab/secondyear/spim/node10.html
/*
 * Symbolic register names for 32 bit ABI
 */
#define zero    $0      /* wired zero */
#define AT      $1      /* assembler temp  - uppercase because of ".set at" */
#define v0      $2      /* return value */
#define v1      $3
#define a0      $4      /* argument registers */
#define a1      $5
#define a2      $6
#define a3      $7
#define t0      $8      /* caller saved */
#define t1      $9
#define t2      $10
#define t3      $11
#define t4      $12
#define ta0     $12
#define t5      $13
#define ta1     $13
#define t6      $14
#define ta2     $14
#define t7      $15
#define ta3     $15
#define s0      $16     /* callee saved */
#define s1      $17
#define s2      $18
#define s3      $19
#define s4      $20
#define s5      $21
#define s6      $22
#define s7      $23
#define t8      $24     /* caller saved */
#define t9      $25
#define jp      $25     /* PIC jump register */
#define k0      $26     /* kernel scratch */
#define k1      $27
#define gp      $28     /* global pointer */
#define sp      $29     /* stack pointer */
#define fp      $30     /* frame pointer */
#define s8      $30     /* same like fp! */
#define ra      $31     /* return address */


#endif /* MIPS_REGDEF_H_ */

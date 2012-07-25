/**
 * @file
 *
 * @brief
 *
 * @date 26.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_TRAPS_H_
#define MIPS_TRAPS_H_


#ifdef __ASSEMBLER__
#define RVECENT(f,n) \
   b f; nop

#define XVECENT(f,bev) \
   b f     ;           \
   li k0,bev
#endif

#endif /* MIPS_TRAPS_H_ */

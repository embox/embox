/**
 * @file
 *
 * @brief
 *
 * @date 26.06.2012
 * @author Anton Bondarev
 */

#ifndef TRAPS_H_
#define TRAPS_H_

#define RVECENT(f,n) \
   b f; nop

#define XVECENT(f,bev) \
   b f     ;           \
   li k0,bev

#endif /* TRAPS_H_ */

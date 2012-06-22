/**
 * @file
 *
 * @brief
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_ASM_IPL_H_
#define MIPS_ASM_IPL_H_

typedef unsigned int __ipl_t;

static inline void ipl_init(void) {
}

static inline __ipl_t ipl_save(void) {
	return 0;
}

static inline void ipl_restore(__ipl_t ipl) {
}

#endif /* MIPS_ASM_IPL_H_ */

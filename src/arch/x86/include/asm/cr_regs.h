/**
 * @file
 *
 * @date Jul 22, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_ARCH_X86_MMU_CR_REGS_H_
#define SRC_ARCH_X86_MMU_CR_REGS_H_

/*
 * Work with registers
 */

static inline void set_cr3(unsigned int pagedir) {
	asm ("mov %0, %%cr3": :"r" (pagedir));
}

static inline unsigned int get_cr3(void) {
	unsigned int _cr3;
	asm ("mov %%cr3, %0":"=r" (_cr3));
	return _cr3;
}

static inline void set_cr0(unsigned int val) {
	asm ("mov %0, %%cr0" : :"r" (val));
}

static inline unsigned int get_cr0(void) {
	unsigned int _cr0;
	asm ("mov %%cr0, %0":"=r" (_cr0));
	return _cr0;
}

static inline void set_cr4(unsigned int val) {
	asm ("mov %0, %%cr4" : :"r" (val));
}

static inline unsigned int get_cr4(void) {
	unsigned int _cr4;
	asm ("mov %%cr2, %0":"=r" (_cr4):);
	return _cr4;
}


static inline unsigned int get_cr2(void) {
	unsigned int _cr2;

	asm ("mov %%cr2, %0":"=r" (_cr2):);
	return _cr2;
}

#endif /* SRC_ARCH_X86_MMU_CR_REGS_H_ */

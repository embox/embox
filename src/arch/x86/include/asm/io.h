/**
 * @file
 * @brief The definitions for the x86 IO instructions.
 *
 * @date 12.12.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef X86_IO_H_
#define X86_IO_H_

#include <stdint.h>

static inline void farpokeb( unsigned sel, void *off, char v ) {
	__asm__ __volatile__ (
		"push %%fs\n\t"
		"mov  %0, %%fs\n\t"
		"movb %2, %%fs:(%1)\n\t"
		"pop %%fs"
		: : "g"(sel), "r"(off), "r"(v)
	);
}

static inline unsigned farpeekl( unsigned sel, void * off ) {
	unsigned ret;
	__asm__ __volatile__ (
		"push %%fs\n\t"
		"mov  %1, %%fs\n\t"
		"mov  %%fs:(%2), %0\n\t"
		"pop  %%fs"
		: "=r"(ret) : "g"(sel), "r"(off)
	);
	return ret;
}

static inline uint8_t in8(unsigned long port) {
	uint8_t tmp;
	__asm__ __volatile__(
		"inb %w1, %b0"
		: "=a"(tmp)
		: "Nd"(port)
	);
	return tmp;
}

static inline uint16_t in16(unsigned long port) {
	uint16_t tmp;
	__asm__ __volatile__(
		"inw %w1, %w0"
		: "=a"(tmp)
		: "Nd"(port)
	);
	return tmp;
}

static inline uint32_t in32(unsigned long port) {
	uint32_t tmp;
	__asm__ __volatile__(
		"in %w1, %0"
		: "=a"(tmp)
		: "Nd"(port)
	);
	return tmp;
}

#define inb(port)     in8(port)
#define inb_p(port)   in8(port)
#define inw(port)     in16(port)
#define inl(port)     in32(port)

#define out8(val, port)                  \
	__asm__ __volatile__(            \
		"outb %b0, %w1"          \
		: : "a"(val), "Nd"(port) \
	)

#define out16(val, port)                 \
	__asm__ __volatile__(            \
		"outw %w0, %w1"          \
		: : "a"(val), "Nd"(port) \
	)

#define out32(val, port)                 \
	__asm__ __volatile__(            \
		"out %0, %w1"            \
		: : "a"(val), "Nd"(port) \
	)

#define outb(value, port)   out8(value, port)
#define outw(value, port)   out16(value, port)
#define outl(value, port)   out32(value, port)

// so would easier
#define __INS(suffix) \
	static inline void ins##suffix(unsigned short port, void *addr, \
			unsigned long count) {                  \
		__asm__ __volatile__ (                          \
			"cld ; rep ; ins" #suffix               \
			: "=D" (addr), "=c" (count)             \
			: "d" (port), "0" (addr), "1" (count)   \
		);                                              \
	}

#define __OUTS(suffix) \
	static inline void outs##suffix(unsigned short port, const void *addr, \
			unsigned long count) {                \
		__asm__ __volatile__ (                        \
			"cld ; rep ; outs" #suffix            \
			: "=S" (addr), "=c" (count)           \
			: "d" (port), "0" (addr), "1" (count) \
		);                                            \
	}

__INS(b)
__INS(w)
__INS(l)

__OUTS(b)
__OUTS(w)
__OUTS(l)

#undef __INS
#undef __OUTS

#endif /* X86_IO_H_ */

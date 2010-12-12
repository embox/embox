/**
 * @file
 *
 * @brief
 *
 * @date 12.12.2010
 * @author Anton Bondarev
 */

#ifndef ASM_X86_IO_H_
#define ASM_X86_IO_H_

static inline uint8_t inb(uint16_t port) {
	uint8_t value;
	__asm__ __volatile__ ("inb %w1, %b0" : "=a"(value) : "Nd" (port));
	return value;
}

static inline void outb(uint8_t value, uint16_t port) {
	__asm__ __volatile__ ("outb %b0, %w1" : : "a" (value), "Nd" (port));
}

#endif /* ASM_X86_IO_H_ */

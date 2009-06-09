#ifndef __CACHE_H__
#define __CACHE_H__

#include "types.h"
#include "leon.h"

inline static void cache_enable() {
	l_regs->cachectrl |= 0xF;
	__asm__ ("flush");
}

inline static void cache_disable() {
	l_regs->cachectrl &= ~0xF;
	__asm__ ("flush");
}

inline static void cache_instr_enable() {
	l_regs->cachectrl |= 0x3;
	__asm__ ("flush");
}

inline static void cache_instr_disable() {
	l_regs->cachectrl &= ~0x3;
	__asm__ ("flush");
}

inline static void cache_data_enable() {
	l_regs->cachectrl |= 0xC;
	__asm__ ("flush");
}

inline static void cache_data_disable() {
	l_regs->cachectrl &= ~0xC;
	__asm__ ("flush");
}

#endif // ifndef __CACHE_H__

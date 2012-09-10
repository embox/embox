/**
 * @file
 * @brief implementaton of arm eabi specs for long long operations
 *
 * @author  Anton Kozlov
 * @date    02.07.2012
 */

#include <types.h>

extern uint64_t __udivdi3(uint64_t num, uint64_t den);
extern uint64_t __umoddi3(uint64_t num, uint64_t den);
extern uint64_t __divdi3(int64_t num, int64_t den);
extern uint64_t __moddi3(int64_t num, int64_t den);

void __aeabi_uldivmod(uint64_t u, uint64_t v) {
	uint64_t res;
	uint64_t mod;
	res = __udivdi3(u, v);
	mod = __umoddi3(u, v);
	{
		register uint32_t r0 __asm__ ("r0") = (res & 0xFFFFFFFF);
		register uint32_t r1 __asm__ ("r1") = (res >> 32);
		register uint32_t r2 __asm__ ("r2") = (mod & 0xFFFFFFFF);
		register uint32_t r3 __asm__ ("r3") = (mod >> 32);
		asm volatile(""
			 : "+r"(r0), "+r"(r1), "+r"(r2),"+r"(r3)  // output
			 : "r"(r0), "r"(r1), "r"(r2), "r"(r3));   // input
		return ;//r0;
	}
}

void __aeabi_ldivmod(int64_t u, int64_t v) {
	uint64_t res;
	uint64_t mod;
	res = __divdi3(u, v);
	mod = __moddi3(u, v);
	{
		register uint32_t r0 __asm__ ("r0") = (res & 0xFFFFFFFF);
		register uint32_t r1 __asm__ ("r1") = (res >> 32);
		register uint32_t r2 __asm__ ("r2") = (mod & 0xFFFFFFFF);
		register uint32_t r3 __asm__ ("r3") = (mod >> 32);
		asm volatile(""
			 : "+r"(r0), "+r"(r1), "+r"(r2),"+r"(r3)  // output
			 : "r"(r0), "r"(r1), "r"(r2), "r"(r3));   // input
		return ;//r0;
	}
}

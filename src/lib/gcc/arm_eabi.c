/**
 * @file
 * @brief implementaton of ARM EABI
 *
 * @author  Anton Kozlov
 * @date    02.07.2012
 */

#if defined(__ARMEL__) && (__ARMEL__==1)

#include <stdint.h>

extern uint64_t __udivdi3(uint64_t num, uint64_t den);
extern uint64_t __umoddi3(uint64_t num, uint64_t den);
extern uint64_t __divdi3(int64_t num, int64_t den);
extern uint64_t __moddi3(int64_t num, int64_t den);

extern int __divsi3(int op1, int op2);
unsigned int __udivsi3(unsigned int op1, unsigned int op2);

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

int __aeabi_idiv(int num, int denom) {
	return __divsi3(num, denom);
}

unsigned int __aeabi_uidiv(unsigned int num, unsigned int denom) {
	return __udivsi3(num, denom);
}

void __aeabi_uidivmod(unsigned int num, unsigned int denom) {
	uint32_t res = __udivsi3(num, denom);
	uint32_t mod = num - res * denom;
	{
		register uint32_t r0 __asm__ ("r0") = res;
		register uint32_t r1 __asm__ ("r1") = mod;
		asm volatile(""
			 : "+r"(r0), "+r"(r1)
			 : "r"(r0), "r"(r1));   // input
		return ;//r0;
	}
}

void __aeabi_idivmod(int num, int denom) {
	int32_t res = __divsi3(num, denom);
	int32_t mod = num - res * denom;
	{
		register int32_t r0 __asm__ ("r0") = res;
		register int32_t r1 __asm__ ("r1") = mod;
		asm volatile(""
			 : "+r"(r0), "+r"(r1)
			 : "r"(r0), "r"(r1));   // input
		return ;//r0;
	}
}

#endif // defined(__ARMEL__) && (__ARMEL__==1)

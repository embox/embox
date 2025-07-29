/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 30.07.25
 */

#include <stdint.h>

#include <asm/csr.h>
#include <framework/mod/options.h>
#include <hal/cpu.h>
#include <hal/reg.h>
#include <riscv/clint.h>
#include <riscv/smode.h>

#define CLINT_BASE OPTION_GET(NUMBER, base_addr)

#if NCPU == 1
#define CLINT_MTIMECMP(hart_id) (CLINT_BASE + 0x4000 + 0)
#else
#define CLINT_MTIMECMP(hart_id) (CLINT_BASE + 0x4000 + hart_id * 8)
#endif
#define CLINT_MTIME (CLINT_BASE + 0xbff8)

uint64_t clint_get_time(void) {
#if __riscv_xlen == 64
#if RISCV_SMODE
	return csr_read(time);
#else
	return REG64_LOAD(CLINT_MTIME);
#endif
#else /* __riscv_xlen */
	uint32_t hi, lo;

#if RISCV_SMODE
	do {
		hi = csr_read(timeh);
		lo = csr_read(time);
	} while (hi != csr_read(timeh));
#else
	do {
		hi = REG32_LOAD(CLINT_MTIME + 4);
		lo = REG32_LOAD(CLINT_MTIME);
	} while (hi != REG32_LOAD(CLINT_MTIME + 4));
#endif

	return ((uint64_t)hi << 32) | lo;
#endif /* __riscv_xlen */
}

void clint_set_timer(uint64_t val) {
	clint_set_timer_cpu(cpu_get_id(), val);
}

void clint_set_timer_cpu(unsigned int hart_id, uint64_t val) {
#if RISCV_SMODE
	sbi_ecall(val, 0, 0, 0, 0, 0, SBI_EXT_TIME_SET_TIMER, SBI_EXT_TIME);
#else
#if __riscv_xlen == 64
	REG64_STORE(CLINT_MTIMECMP(hart_id), val);
#else
	/* Set high word to max first to avoid triggering interrupt */
	REG32_STORE(CLINT_MTIMECMP(hart_id) + 4, 0xffffffff);
	REG32_STORE(CLINT_MTIMECMP(hart_id), (uint32_t)(val & 0xffffffff));
	REG32_STORE(CLINT_MTIMECMP(hart_id) + 4, (uint32_t)(val >> 32));
#endif
#endif
}

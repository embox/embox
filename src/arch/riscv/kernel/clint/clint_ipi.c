/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 30.07.25
 */

#include <stdint.h>
#include <sys/cdefs.h>

#include <asm/csr.h>
#include <framework/mod/options.h>
#include <hal/cpu.h>
#include <hal/reg.h>
#include <riscv/smode.h>
#include <util/log.h>

#define CLINT_BASE OPTION_GET(NUMBER, base_addr)

#if NCPU == 1
#define CLINT_MSIP(hart_id) (CLINT_BASE + 0)
#else
#define CLINT_MSIP(hart_id) (CLINT_BASE + hart_id * 4)
#endif

void clint_send_ipi(unsigned int hart_id) {
#if RISCV_SMODE
	sbi_ecall(1UL, hart_id, 0, 0, 0, 0, SBI_EXT_IPI_SEND_IPI, SBI_EXT_IPI);
#else
	REG32_STORE(CLINT_MSIP(hart_id), 1);
#endif
}

void clint_clear_ipi(void) {
#if RISCV_SMODE
	csr_clear(CSR_IP, CSR_IP_SIP);
#else
	REG32_STORE(CLINT_MSIP(cpu_get_id()), 0);
#endif
}

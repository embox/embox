/**
 * @file
 *
 * @brief Implementation of the RISC-V Core Local Interruptor (CLINT) for interrupt control, including support for SiFive multi-hart architectures.
 *
 * @date 05.07.2024
 * @authored by Suraj Ravindra Sonawane
 */

#include <asm/regs.h>
#include <asm/interrupts.h>
#include <hal/cpu.h>
#include <hal/reg.h>
#include <sbi/sbi.h>
#include <drivers/irqctrl.h>

#define HAS64BITMMIO	OPTION_GET(BOOLEAN, has64bitmmio)
#define NO_MTIME_REG	OPTION_GET(BOOLEAN, no_mtime_reg)
#define CLINT_ADDR      OPTION_GET(NUMBER, base_addr)
#define MSIP_OFFSET     OPTION_GET(NUMBER, msip_offset)
#define MTIMECMP_OFFSET OPTION_GET(NUMBER, mtimecmp_offset)
#define MTIME_OFFSET    OPTION_GET(NUMBER, mtime_offset)

#define MSIP_ADDR(hart)     (CLINT_ADDR + MSIP_OFFSET + ((hart) * 4))
#define MTIMECMP_ADDR(hart) (CLINT_ADDR + MTIMECMP_OFFSET + ((hart) * 8))

/* time counter register is read only*/
#define MTIME_ADDR (CLINT_ADDR + MTIME_OFFSET)

#if __riscv_xlen != 32 && !HAS64BITMMIO

#define READ_64BIT(addr) ({		\
	uint32_t lo, hi;		\
	lo = REG32_LOAD(addr);		\
	hi = REG32_LOAD(addr + 4);	\
	(((uint64_t)hi << 32) | lo);	\
})

#define WRITE_64BIT(addr, value) do {			\
	REG32_STORE(addr, value & 0xFFFFFFFF); \
	REG32_STORE(addr + 4, (value >> 32) & 0xFFFFFFFF); \
} while(0)

#else /* __riscv_xlen == 32 || !HAS64BITMMIO */

#define WRITE_64BIT(addr, value)	REG64_STORE(addr, value)
#define READ_64BIT(addr)		REG64_LOAD(addr)

#endif /* __riscv_xlen == 32 || !HAS64BITMMIO */

#if !SMODE

/* In SMP case, each CPU will call this function independently */
int clint_init(void) {
	int hartid = cpu_get_id();
	REG32_STORE(MSIP_ADDR(hartid), 0); /* Clear Pending bit */
	WRITE_64BIT(MTIMECMP_ADDR(hartid), (uint64_t)-1); /* Disable timer interrupt */
	enable_software_interrupts();
	return 0;
}

void configure_soft_int(uint8_t value, int hart_id) {
	REG32_STORE(MSIP_ADDR(hart_id), value & 1);
}

void set_timecmp(uint64_t value, int hart_id) {
	WRITE_64BIT(MTIMECMP_ADDR(hart_id), value);
}

uint64_t get_timecmp(int hart_id) {
	return READ_64BIT(MTIMECMP_ADDR(hart_id));
}

uint64_t get_current_time(void) {
#if NO_MTIME_REG
  #if __riscv_xlen == 32
        uint32_t hi, lo;

        do {
                hi = read_csr(TIMEH_REG);
                lo = read_csr(TIME_REG);
        } while (hi != read_csr(TIMEH_REG));

        return ((uint64_t)hi << 32) | lo;
  #else /* __riscv_xlen == 64 */
        return read_csr(TIME_REG);
  #endif
#else
	return READ_64BIT(MTIME_ADDR);
#endif /* NO_MTIME_REG */
}

#else /* SMODE */

int clint_init(void) {
	sbi_set_timer((uint64_t)-1);
	return 0;
}

/* TODO */
void configure_soft_int(uint8_t value, int hart_id) {}

void set_timecmp(uint64_t timeval, int hart_id) {
	sbi_set_timer(timeval);
}

uint64_t get_current_time(void) {
#if __riscv_xlen == 32
	uint32_t hi, lo;

	do {
		hi = read_csr(TIMEH_REG);
		lo = read_csr(TIME_REG);
	} while (hi != read_csr(TIMEH_REG));

	return ((uint64_t)hi << 32) | lo;
#else /* __riscv_xlen == 64 */
	return read_csr(TIME_REG);
#endif
}

#endif

/**
 * @file
 * @brief
 *
 * @date 20.05.2025
 * @author Zeng Zixian
 */

#include <sbi/sbi.h>

void sbi_set_timer(uint64_t timeval) {
#if __riscv_xlen == 32
	sbi_ecall(stime_value, stime_value >> 32, 0, 0, 0, 0,
		  SBI_EXT_TIME_SET_TIMER, SBI_EXT_TIME);
#else
	sbi_ecall(stime_value, 0, 0, 0, 0, 0,
		  SBI_EXT_TIME_SET_TIMER, SBI_EXT_TIME);
#endif

}

void sbi_send_ipi(uint64_t hartid) {
	int result;
	struct sbiret ret = {0};

	ret = sbi_ecall(1UL, hartid, 0, 0, 0, 0,
			SBI_EXT_IPI_SEND_IPI, SBI_EXT_IPI);
	if (ret.error) {
		log_error("%s: hbase = [%lu] failed (error [%d])\n",
			  __func__, hartid, result);
	}

}

int sbi_hsm_hart_start(unsigned long hartid, unsigned long addr,
			unsigned long mode) {
	struct sbiret ret;

	ret = sbi_ecall(hartid, addr, mode, 0, 0, 0,
			SBI_EXT_HSM_HART_START, SBI_EXT_HSM);
	return ret.error;
}

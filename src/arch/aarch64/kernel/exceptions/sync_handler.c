/**
 * @brief Synchronous exception handler
 *
 * @date 15.12.22
 * @author Aleksey Zhmulin
 */
#include <compiler.h>
#include <inttypes.h>
#include <stdint.h>

#include <hal/reg.h>
#include <util/field.h>
#include <util/log.h>

#include "exception.h"

static void print_abort_syndrome(uint32_t syndrome) {
	int el;
	unsigned dfsc;

	el = 1;
	dfsc = syndrome & 0b111111;

	switch (dfsc) {
	case 0b000000:
		el = 0;
	case 0b000001:
		log_raw(LOG_EMERG, "Address Size fault (EL%i)\n", el);
		break;

	case 0b000100:
	case 0b000101:
	case 0b000110:
	case 0b000111:
		log_raw(LOG_EMERG, "Translation fault (level %u)\n", dfsc & 0b11);
		break;

	case 0b001000:
		el = 0;
	case 0b001001:
		log_raw(LOG_EMERG, "Access Flag fault (EL%i)\n", el);
		break;

	case 0b001100:
		el = 0;
	case 0b001101:
		log_raw(LOG_EMERG, "Permission fault (EL%i)\n", el);
		break;

	case 0b010000:
		log_raw(LOG_EMERG, "External abort\n");
		break;

	case 0b011000:
		log_raw(LOG_EMERG, "Parity error on a memory access\n");
		break;

	case 0b010100:
		el = 0;
	case 0b010101:
		log_raw(LOG_EMERG,
		    "External abort on a translation table walk (EL%i)\n", el);
		break;

	case 0b011100:
		el = 0;
	case 0b011101:
		log_raw(LOG_EMERG,
		    "Parity error on a memory access on a translation table walk "
		    "(EL%i)\n",
		    el);
		break;

	case 0b100001:
		log_raw(LOG_EMERG, "Alignment fault\n");
		break;

	case 0b110000:
		log_raw(LOG_EMERG, "TLB Conflict fault\n");
		break;

	default:
		log_raw(LOG_EMERG, "Unknown fault (DFSC = %#x)\n", dfsc);
		break;
	}
}

void _NORETURN aarch64_sync_handler(struct excpt_context *ctx) {
	uint32_t esr;
	uint32_t class;
	uint32_t syndrome;

	esr = ARCH_REG_LOAD(ESR_EL1);
	class = FIELD_GET(esr, ESR_ELn_EC);
	syndrome = FIELD_GET(esr, ESR_ELn_ISS);

	switch (class) {
	case ESR_ELn_EC_INST_ABT:
		log_raw(LOG_EMERG, "\nInstruction abort exception!\n");
		print_abort_syndrome(syndrome);
		log_raw(LOG_EMERG, "FAR_EL1 = %#" PRIx64 "\n",
		    (uint64_t)ARCH_REG_LOAD(FAR_EL1));
		break;

	case ESR_ELn_EC_DATA_ABT:
		log_raw(LOG_EMERG, "\nData abort exception!\n");
		print_abort_syndrome(syndrome);
		log_raw(LOG_EMERG, "FAR_EL1 = %#" PRIx64 "\n",
		    (uint64_t)ARCH_REG_LOAD(FAR_EL1));
		break;

	default:
		log_raw(LOG_EMERG, "\nSynchronous exception!\n");
		log_raw(LOG_EMERG, "ESR_EL1 = %" PRIx32 "\n", esr);
		break;
	}

	aarch64_print_excpt_context(ctx);
	while (1) {};
}

/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @author  Alexander Kalmuk
 * @date    18.03.2015
 */
#include <inttypes.h>

#include <kernel/panic.h>
#include <kernel/printk.h>
#include <hal/context.h>
#include <hal/reg.h>

/* SCB - System Control Block */
#define SCB_BASE 0xe000ed00
#define SCB_MEM_FAULT_STATUS   (SCB_BASE + 0x28)
# define SCB_MEM_FAULT_MMARVALID (1 << 7)
#define SCB_BUS_FAULT_STATUS   (SCB_BASE + 0x29)
# define SCB_BUS_FAULT_BFARVALID (1 << 7)
#define SCB_USAGE_FAULT_STATUS (SCB_BASE + 0x2A)
#define SCB_HARD_FAULT_STATUS  (SCB_BASE + 0x2C)
#define SCB_MEM_FAULT_ADDRESS  (SCB_BASE + 0x34)
#define SCB_BUS_FAULT_ADDRESS  (SCB_BASE + 0x38)

static void print_fault_status(void) {
	uint32_t bus_fault_status, mem_fault_status, usage_fault_status;

	mem_fault_status = REG8_LOAD(SCB_MEM_FAULT_STATUS);
	bus_fault_status = REG8_LOAD(SCB_BUS_FAULT_STATUS);
	usage_fault_status = REG16_LOAD(SCB_USAGE_FAULT_STATUS);

	printk("MemManage Fault Status register = 0x%02" PRIx32 "\n", mem_fault_status);
	if (mem_fault_status & SCB_MEM_FAULT_MMARVALID) {
		printk("  MemManage Fault Address = 0x%08" PRIx32 "\n",
				REG32_LOAD(SCB_MEM_FAULT_ADDRESS));
	}
	printk("Bus Fault Status register = 0x%02" PRIx32 "\n", bus_fault_status);
	if (bus_fault_status & SCB_BUS_FAULT_BFARVALID) {
		printk("  Bus Fault Address = 0x%08" PRIx32 "\n",
				REG32_LOAD(SCB_BUS_FAULT_ADDRESS));
	}
	printk("Usage Fault Status register = 0x%04" PRIx32 "\n", usage_fault_status);
	printk("Hard Fault Status register = 0x%08" PRIx32 "\n",
			REG32_LOAD(SCB_HARD_FAULT_STATUS));
}

/* Print exception info and return */
void exc_default_handler(struct exc_saved_base_ctx *ctx, int ipsr) {
	printk("\nEXCEPTION (0x%x):\n"
			"Exception saved context:\n"
			"  r0=0x%08" PRIx32 " r1=0x%08" PRIx32 " r2=0x%08" PRIx32 " r3=0x%08" PRIx32 "\n"
			"  r12=0x%08" PRIx32 " lr=0x%08" PRIx32 " pc(ret)=0x%08" PRIx32 " xPSR=0x%08" PRIx32 "\n",
			ipsr,
			ctx->r[0], ctx->r[1], ctx->r[2], ctx->r[3],
			ctx->r[4], ctx->lr, ctx->pc, ctx->psr);

	print_fault_status();
	panic("\n%s\n", __func__);
}

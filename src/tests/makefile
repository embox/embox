include $(SCRIPTS_DIR)/autoconf

OBJS-y                               += express_tests.o
OBJS-$(MONITOR_TEST_CPU_CONTEXT)     += test_cpu_context.o
OBJS-$(MONITOR_TEST_IRQ_AVAILABILITY)+= test_irq_availability.o
OBJS-$(MONITOR_TEST_SOFT_TRAPS)      += test_soft_traps.o
OBJS-$(MONITOR_TEST_MMU)             += test_mmu.o
OBJS-$(MONITOR_TEST_CHECKSUM)        += test_checksum.o

include $(SRC_DIR)/common.mk

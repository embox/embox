include $(SCRIPTS_DIR)/autoconf

OBJS-y += tests_iterator.o
OBJS-$(MONITOR_TESTS_CPU_CONTEXT)     += test_cpu_context.o
OBJS-$(MONITOR_TESTS_IRQ_AVAILABILITY)+= test_irq_availability.o
OBJS-$(MONITOR_TESTS_SOFT_TRAPS)      += test_soft_traps.o
OBJS-$(MONITOR_TESTS_MMU)             += test_mmu.o
OBJS-$(MONITOR_TESTS_CHECKSUM)        += test_checksum.o

include $(SRC_DIR)/common.mk

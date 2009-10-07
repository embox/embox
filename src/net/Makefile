include $(SCRIPTS_DIR)/autoconf

SUBDIRS-$(MONITOR_NETWORK) += core/
SUBDIRS-$(MONITOR_NETWORK) += ethernet/
SUBDIRS-$(MONITOR_NETWORK) += ipv4/
OBJS-$(MONITOR_NETWORK) += ipaux.o
OBJS-$(MONITOR_NETWORK) += ipconfig.o
OBJS-$(MONITOR_NETWORK) += socket.o

include $(SCRIPTS_DIR)/common.mk

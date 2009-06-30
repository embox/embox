include $(SCRIPTS_DIR)/autoconf

SUBDIRS-$(MONITOR_NET)               += ipv4
SUBDIRS-$(MONITOR_NET_ETH)           += ethernet
SUBDIRS-$(MONITOR_NET)               += core

OBJS-$(MONITOR_NET_IPAUX)            += ipaux.o
OBJS-$(MONITOR_NET_SOCKET)           += socket.o

include $(SRC_DIR)/common.mk

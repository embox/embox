include $(SCRIPTS_DIR)/autoconf

SUBDIRS-$(MONITOR_CONIO) += console
SUBDIRS-$(MONITOR_CONIO) += terminal

OBJS-y:= printf.o \
         scanf.o

OBJS-$(MONITOR_CONIO)    += shell.o

include $(SRC_DIR)/common.mk

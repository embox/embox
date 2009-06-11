include $(SCRIPTS_DIR)/autoconf

SUBDIRS-$(MONITOR_CONIO_CONSOLE)  += console
SUBDIRS-$(MONITOR_CONIO_TERMINAL) += terminal

OBJS-y:= printf.o \
         scanf.o \
         shell.o

include $(SRC_DIR)/common.mk

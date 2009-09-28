include $(SCRIPTS_DIR)/autoconf

SUBDIRS-$(MONITOR_CONIO) += console

OBJS-y:= printf.o \
         scanf.o \
         getopt.o

OBJS-$(MONITOR_CONIO)    += shell.o

include $(SRC_DIR)/common.mk

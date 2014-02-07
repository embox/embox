
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

CC ?= gcc

BUILD_DIR := $(ROOT_DIR)/build/external_build

all : $(BUILD_DIR)/lib.a $(BUILD_DIR)/libtool

$(BUILD_DIR)/lib.o : lib.c | $(BUILD_DIR)
	$(EMBOX_CROSS_COMPILE)gcc $(EMBOX_CFLAGS) $(EMBOX_CPPFLAGS) -c -o $@ $<

$(BUILD_DIR)/lib.a : $(BUILD_DIR)/lib.o
	ar rcs $@ $<

$(BUILD_DIR)/libtool : libtool.c | $(BUILD_DIR)
	$(CC) -o $@ $<

$(BUILD_DIR):
	mkdir -p $@



ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

BUILD_DIR := $(ROOT_DIR)/build/external_build

all : $(BUILD_DIR)/app.o

$(BUILD_DIR)/app.o : app.c | $(BUILD_DIR)
	$(BUILD_DIR)/libtool
	$(EMBOX_CROSS_COMPILE)gcc $(EMBOX_CFLAGS) $(EMBOX_CPPFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/app.o : app.c


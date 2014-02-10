
CC ?= gcc

all : $(BUILD_DIR)/lib.a $(BUILD_DIR)/libtool

$(BUILD_DIR)/lib.o : lib.c | $(BUILD_DIR)
	$(EMBOX_CROSS_COMPILE)gcc $(EMBOX_CFLAGS) $(EMBOX_CPPFLAGS) -c -o $@ $<

$(BUILD_DIR)/lib.a : $(BUILD_DIR)/lib.o
	ar rcs $@ $<

$(BUILD_DIR)/libtool : libtool.c | $(BUILD_DIR)
	$(CC) -o $@ $<

$(BUILD_DIR):
	mkdir -p $@


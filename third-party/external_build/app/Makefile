
all : $(BUILD_DIR)/app.o

LIB_TOOL = $(EXTERNAL_BUILD_DIR)/third_party/external_build/lib/libtool

$(BUILD_DIR)/app.o : app.c | $(BUILD_DIR)
	$(LIB_TOOL)
	$(EMBOX_CROSS_COMPILE)gcc $(EMBOX_CFLAGS) $(EMBOX_CPPFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/app.o : app.c



all : $(MOD_BUILD_DIR)/app.o

LIB_TOOL = $(EXTERNAL_BUILD_DIR)/third_party/external_build/lib/libtool

$(MOD_BUILD_DIR)/app.o : app.c | $(MOD_BUILD_DIR)
	$(LIB_TOOL)
	$(EMBOX_CROSS_COMPILE)gcc $(EMBOX_CFLAGS) $(EMBOX_CPPFLAGS) -c -o $@ $<

$(MOD_BUILD_DIR):
	mkdir -p $@

$(MOD_BUILD_DIR)/app.o : app.c


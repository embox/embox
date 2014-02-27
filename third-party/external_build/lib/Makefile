all : $(BUILD_DIR)/lib.a $(BUILD_DIR)/lib.h $(BUILD_DIR)/libtool

$(BUILD_DIR)/lib.o : lib.c | $(BUILD_DIR)
	$(EMBOX_CROSS_COMPILE)gcc $(EMBOX_CFLAGS) $(EMBOX_CPPFLAGS) -c -o $@ $<

$(BUILD_DIR)/lib.a : $(BUILD_DIR)/lib.o
	$(EMBOX_CROSS_COMPILE)ar rcs $@ $<

$(BUILD_DIR)/lib.h :
	echo '#ifndef __LIB_H_' > $@
	echo '#define __LIB_H_' >> $@
	echo '#include <lib2.h>' >> $@
	echo '#define EXTERNAL_LIB_VAL EXTERNAL_LIB2_VAL' >> $@
	echo '#endif /* __LIB_H_ */' >> $@

$(BUILD_DIR)/libtool : libtool.c | $(BUILD_DIR)
	$(EMBOX_CROSS_COMPILE)gcc -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

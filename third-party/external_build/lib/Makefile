all : $(MOD_BUILD_DIR)/lib.a $(MOD_BUILD_DIR)/lib.h $(MOD_BUILD_DIR)/libtool

$(MOD_BUILD_DIR)/lib.o : lib.c | $(MOD_BUILD_DIR)
	$(EMBOX_CROSS_COMPILE)gcc $(EMBOX_CFLAGS) $(EMBOX_CPPFLAGS) -c -o $@ $<

$(MOD_BUILD_DIR)/lib.a : $(MOD_BUILD_DIR)/lib.o
	$(EMBOX_CROSS_COMPILE)ar rcs $@ $<

$(MOD_BUILD_DIR)/lib.h :
	echo '#ifndef __LIB_H_' > $@
	echo '#define __LIB_H_' >> $@
	echo '#include <lib2.h>' >> $@
	echo '#define EXTERNAL_LIB_VAL EXTERNAL_LIB2_VAL' >> $@
	echo '#endif /* __LIB_H_ */' >> $@

$(MOD_BUILD_DIR)/libtool : libtool.c | $(MOD_BUILD_DIR)
	$(EMBOX_CROSS_COMPILE)gcc -o $@ $<

$(MOD_BUILD_DIR):
	mkdir -p $@

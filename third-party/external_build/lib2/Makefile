all : $(MOD_BUILD_DIR)/lib2.h

$(MOD_BUILD_DIR)/lib2.h : | $(MOD_BUILD_DIR)
	echo '#ifndef __LIB2_H_' > $@
	echo '#define __LIB2_H_' >> $@
	echo '#define EXTERNAL_LIB2_VAL 314' >> $@
	echo '#endif /* __LIB2_H_ */' >> $@

$(MOD_BUILD_DIR):
	mkdir -p $@

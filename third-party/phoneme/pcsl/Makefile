
include $(EXTBLD_LIB)

JAVACALL_OUTPUT_DIR := $(BUILD_DIR)/../javacall
PCSL_OUTPUT_DIR := $(BUILD_DIR)

cppflags := $(filter -I%,$(EMBOX_CPPFLAGS))
cppflags += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
cppflags += $(filter -D__EMBOX__,$(EMBOX_CPPFLAGS))
cppflags += $(subst \",\',$(filter -D\"__impl_x%,$(EMBOX_CPPFLAGS)))
cflags   := -fno-stack-protector
cflags   += $(filter -m32,$(EMBOX_CFLAGS))

$(CONFIGURE) :
	cd $(BUILD_DIR) && ( \
		ln -s ../src/phoneme/pcsl src \
	)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR) && ( \
		make -C src MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			USE_DEBUG=true USE_VERBOSE_MAKE=" true" \
			PCSL_PLATFORM=javacall_i386_gcc \
			PCSL_OUTPUT_DIR=$(PCSL_OUTPUT_DIR) \
			JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
			CFLAGS='$(cppflags) $(cflags)' \
	)
	touch $@

$(INSTALL) :
	ln -s $(BUILD_DIR)/javacall_i386/inc $(PKG_INSTALL_DIR)/include
	ln -s $(BUILD_DIR)/javacall_i386/lib/libpcsl_file.a $(PKG_INSTALL_DIR)/lib
	ln -s $(BUILD_DIR)/javacall_i386/lib/libpcsl_memory.a $(PKG_INSTALL_DIR)/lib
	ln -s $(BUILD_DIR)/javacall_i386/lib/libpcsl_network.a $(PKG_INSTALL_DIR)/lib
	ln -s $(BUILD_DIR)/javacall_i386/lib/libpcsl_print.a $(PKG_INSTALL_DIR)/lib
	ln -s $(BUILD_DIR)/javacall_i386/lib/libpcsl_string.a $(PKG_INSTALL_DIR)/lib
	touch $@

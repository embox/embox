
JAVACALL_OUTPUT_DIR := $(BUILD_DIR)/../javacall
PCSL_OUTPUT_DIR := $(BUILD_DIR)

PCSL_ARCH := $(AUTOCONF_ARCH)

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	cd $(BUILD_DIR) && ( \
		ln -s ../src/phoneme/pcsl src \
	)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR) && ( \
		$(MAKE) -C src MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			USE_DEBUG=true USE_VERBOSE_MAKE=" true" \
			PCSL_PLATFORM=javacall_$(PCSL_ARCH)_gcc \
			PCSL_OUTPUT_DIR=$(PCSL_OUTPUT_DIR) \
			JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
			CC=$(EMBOX_GCC) CROSS_COMPILE=$(EMBOX_CROSS_COMPILE) \
	)
	touch $@

$(INSTALL) :
	ln -s $(BUILD_DIR)/javacall_$(PCSL_ARCH)/inc $(PKG_INSTALL_DIR)/include
	ln -s $(BUILD_DIR)/javacall_$(PCSL_ARCH)/lib/libpcsl_file.a $(PKG_INSTALL_DIR)/lib
	ln -s $(BUILD_DIR)/javacall_$(PCSL_ARCH)/lib/libpcsl_memory.a $(PKG_INSTALL_DIR)/lib
	ln -s $(BUILD_DIR)/javacall_$(PCSL_ARCH)/lib/libpcsl_network.a $(PKG_INSTALL_DIR)/lib
	ln -s $(BUILD_DIR)/javacall_$(PCSL_ARCH)/lib/libpcsl_print.a $(PKG_INSTALL_DIR)/lib
	ln -s $(BUILD_DIR)/javacall_$(PCSL_ARCH)/lib/libpcsl_string.a $(PKG_INSTALL_DIR)/lib
	touch $@


JAVACALL_OUTPUT_DIR := $(MOD_BUILD_DIR)/../javacall
PCSL_OUTPUT_DIR := $(MOD_BUILD_DIR)

PCSL_ARCH := $(EMBOX_ARCH)

$(CONFIGURE) :
	cd $(MOD_BUILD_DIR) && ( \
		ln -s ../src/phoneme/pcsl src \
	)
	touch $@

$(BUILD) :
	cd $(MOD_BUILD_DIR) && ( \
		$(MAKE) -C src MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			USE_DEBUG=true USE_VERBOSE_MAKE=" true" \
			PCSL_PLATFORM=javacall_$(PCSL_ARCH)_gcc \
			PCSL_OUTPUT_DIR=$(PCSL_OUTPUT_DIR) \
			JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
			CC=$(EMBOX_GCC) CROSS_COMPILE=$(EMBOX_CROSS_COMPILE) \
	)
	touch $@

$(INSTALL) :
	ln -s $(MOD_BUILD_DIR)/javacall_$(PCSL_ARCH)/inc $(PKG_INSTALL_DIR)/include
	ln -s $(MOD_BUILD_DIR)/javacall_$(PCSL_ARCH)/lib/libpcsl_file.a $(PKG_INSTALL_DIR)/lib
	ln -s $(MOD_BUILD_DIR)/javacall_$(PCSL_ARCH)/lib/libpcsl_memory.a $(PKG_INSTALL_DIR)/lib
	ln -s $(MOD_BUILD_DIR)/javacall_$(PCSL_ARCH)/lib/libpcsl_network.a $(PKG_INSTALL_DIR)/lib
	ln -s $(MOD_BUILD_DIR)/javacall_$(PCSL_ARCH)/lib/libpcsl_print.a $(PKG_INSTALL_DIR)/lib
	ln -s $(MOD_BUILD_DIR)/javacall_$(PCSL_ARCH)/lib/libpcsl_string.a $(PKG_INSTALL_DIR)/lib
	touch $@


JAVACALL_DIR := $(MOD_BUILD_DIR)/src
JAVACALL_OUTPUT_DIR := $(MOD_BUILD_DIR)
TOOLS_DIR := $(MOD_BUILD_DIR)/../src/phoneme/tools
JDK_DIR := $(shell readlink -f `which javac` | xargs dirname)/..

$(CONFIGURE) :
	cd $(MOD_BUILD_DIR) && ( \
		ln -s ../src/phoneme/javacall src \
	)
	touch $@

$(BUILD) :
	cd $(MOD_BUILD_DIR) && ( \
		$(MAKE) -C src/configuration/stubs/linux MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			USE_DEBUG=true \
			JAVACALL_DIR=$(JAVACALL_DIR) JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
			TOOLS_DIR=$(TOOLS_DIR) \
			JDK_DIR=$(JDK_DIR) \
			CC=$(EMBOX_GCC) \
	)
	touch $@

$(INSTALL) :
	ln -s $(MOD_BUILD_DIR)/inc $(PKG_INSTALL_DIR)/include
	ln -s $(MOD_BUILD_DIR)/obj_g/javautil_unicode.o $(PKG_INSTALL_DIR)
	touch $@

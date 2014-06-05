
include $(EXTBLD_LIB)

JAVACALL_DIR := $(BUILD_DIR)/src
JAVACALL_OUTPUT_DIR := $(BUILD_DIR)
TOOLS_DIR := $(BUILD_DIR)/../src/phoneme/tools
JDK_DIR := $(shell readlink -f `which javac` | xargs dirname)/..

cppflags := $(filter -I%,$(EMBOX_CPPFLAGS))
cppflags += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
cppflags += $(filter -D__EMBOX__,$(EMBOX_CPPFLAGS))
cppflags += $(subst \",\',$(filter -D\"__impl_x%,$(EMBOX_CPPFLAGS)))
cflags   := -fno-stack-protector
cflags   += $(filter -m32,$(EMBOX_CFLAGS))

$(CONFIGURE) :
	cd $(BUILD_DIR) && ( \
		ln -s ../src/phoneme/javacall src \
	)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR) && ( \
		make -C src/configuration/stubs/linux MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			USE_DEBUG=true \
			JAVACALL_DIR=$(JAVACALL_DIR) JAVACALL_OUTPUT_DIR=$(JAVACALL_OUTPUT_DIR) \
			TOOLS_DIR=$(TOOLS_DIR) \
			JDK_DIR=$(JDK_DIR) \
			CFLAGS='$(cppflags) $(cflags)' \
	)
	touch $@

$(INSTALL) :
	ln -s $(BUILD_DIR)/inc $(PKG_INSTALL_DIR)/include
	ln -s $(BUILD_DIR)/obj_g/javautil_unicode.o $(PKG_INSTALL_DIR)
	touch $@

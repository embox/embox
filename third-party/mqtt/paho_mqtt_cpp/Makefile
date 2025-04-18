PKG_NAME	:= paho.mqtt.cpp
PKG_VER  	:= v1.2.0

PKG_ARCHIVE_NAME := $(PKG_NAME)-$(PKG_VER).tar.gz

PKG_SOURCES := https://github.com/eclipse/paho.mqtt.cpp/archive/$(PKG_VER).tar.gz
PKG_MD5 	:= 5a4b7c869e6f20cb85c438456d5b8d12

PKG_PATCHES := patch.txt

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	cd $(PKG_SOURCE_DIR) && ( \
		cmake -DPAHO_WITH_SSL=FALSE \
			-DPAHO_BUILD_STATIC=TRUE \
			-DPAHO_BUILD_SHARED=FALSE \
			-DPAHO_BUILD_DOCUMENTATION=FALSE \
			-DPAHO_BUILD_SAMPLES=FALSE \
			-DCMAKE_CXX_COMPILER_WORKS=1 \
			-DCMAKE_HAVE_LIBC_PTHREAD=1 \
			-DCMAKE_C_FLAGS="" \
			-DCMAKE_CXX_FLAGS="" \
			-DCMAKE_INSTALL_PREFIX=$(PKG_INSTALL_DIR) \
			-DCMAKE_SYSTEM_PROCESSOR:STRING=$(AUTOCONF_TARGET_TRIPLET) \
			-DCMAKE_C_COMPILER:PATH=$(EMBOX_GCC) \
			-DCMAKE_CXX_COMPILER:PATH=$(EMBOX_GXX) \
			-DCMAKE_AR:PATH=$(EMBOX_CROSS_COMPILE)ar\
			-DCMAKE_SYSTEM_NAME:STRING=Generic \
			-DCMAKE_BUILD_TYPE:STRING=None \
			-DCMAKE_VERBOSE_MAKEFILE:BOOL=on \
			-DCMAKE_PREFIX_PATH=$(EXTERNAL_BUILD_DIR)/third_party/mqtt/paho_mqtt_c/libpaho_mqtt3a/install/ \
	)

	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)

	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/libpaho-mqttpp3-static.a $(PKG_INSTALL_DIR)/libpaho-mqttpp3-static.a
	mkdir -p $(PKG_INSTALL_DIR)/include/mqtt
	cp $(PKG_SOURCE_DIR)/src/mqtt/*.h $(PKG_INSTALL_DIR)/include/mqtt/
	touch $@

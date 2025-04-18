PKG_NAME	:= paho.mqtt.c
PKG_VER  	:= v1.3.8

PKG_ARCHIVE_NAME := $(PKG_NAME)-$(PKG_VER).tar.gz

PKG_SOURCES := https://github.com/eclipse/paho.mqtt.c/archive/$(PKG_VER).tar.gz
PKG_MD5 	:= 71b9a3070f543afcb818a8c231be6684

PKG_PATCHES := patch.txt

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	cd $(PKG_INSTALL_DIR) && ( \
		CC=$(EMBOX_GCC) \
		CFLAGS="$(EMBOX_IMPORTED_CFLAGS)" \
		CPPFLAGS="$(EMBOX_IMPORTED_CPPFLAGS)" \
		cmake -DPAHO_WITH_SSL=FALSE \
			-DPAHO_BUILD_STATIC=TRUE \
			-DPAHO_BUILD_SHARED=FALSE \
			-DPAHO_BUILD_DOCUMENTATION=FALSE \
			-DPAHO_BUILD_SAMPLES=FALSE \
			-DPAHO_ENABLE_TESTING=FALSE \
			-DPAHO_ENABLE_CPACK=FALSE \
			-DCMAKE_C_COMPILER_WORKS=1 \
			-DPAHO_HIGH_PERFORMANCE=ON \
			$(PKG_SOURCE_DIR)/ \
	)

	touch $@

$(BUILD) :
	cd $(PKG_INSTALL_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)

	touch $@

$(INSTALL) :
	cp $(PKG_INSTALL_DIR)/src/libpaho-mqtt3c.a $(PKG_INSTALL_DIR)/libpaho-mqtt3c.a
	cp $(PKG_INSTALL_DIR)/src/libpaho-mqtt3a.a $(PKG_INSTALL_DIR)/libpaho-mqtt3a.a
	cp $(PKG_SOURCE_DIR)/src/*.h $(PKG_INSTALL_DIR)/
	touch $@

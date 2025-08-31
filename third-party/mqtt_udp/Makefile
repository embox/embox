PKG_NAME	:= mqtt_udp
PKG_VER  	:= v0.4-1

PKG_SOURCES := https://github.com/dzavalishin/mqtt_udp/archive/$(PKG_VER).tar.gz
PKG_MD5 	:= f881143ae137868bf1ed4308e1844e49

PKG_PATCHES := patch.txt

$(CONFIGURE) :
	cd $(PKG_SOURCE_DIR)/lang/c && aclocal && autoconf && (./configure \
		--host=$(AUTOCONF_TARGET_TRIPLET) \
		--target=$(AUTOCONF_TARGET_TRIPLET) \
		CC=$(EMBOX_GCC) \
		CFLAGS="$(EMBOX_IMPORTED_CFLAGS)" \
		CPPFLAGS="$(EMBOX_IMPORTED_CPPFLAGS)" \
	)

	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR)/lang/c && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)

	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/lang/c/libmqttudp.a $(PKG_INSTALL_DIR)/libmqttudp.a
	cp $(PKG_SOURCE_DIR)/lang/c/mqtt_udp.h $(PKG_INSTALL_DIR)/mqtt_udp.h
	cp $(PKG_SOURCE_DIR)/lang/c/mqtt_udp_defs.h $(PKG_INSTALL_DIR)/mqtt_udp_defs.h

	touch $@

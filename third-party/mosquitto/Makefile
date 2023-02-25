
PKG_NAME := mosquitto
PKG_VER  := 2.0.15

PKG_SOURCES := https://github.com/eclipse/$(PKG_NAME)/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := 1247e425545affe185278a5314e7bbfe

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

# $(CONFIGURE) :
# 	export EMBOX_GCC_LINK=full; \
# 	cd $(PKG_SOURCE_DIR) && ( \
# 		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
# 			--target=$(AUTOCONF_TARGET_TRIPLET) \
# 			--enable-tiny \
# 			--disable-shared \
# 			--disable-static \
# 			--disable-largefile \
# 			--disable-rpath \
# 			--disable-nls \
# 			--disable-extra \
# 			--disable-browser \
# 			--disable-help \
# 			--disable-justify \
# 			--disable-mouse \
# 			--disable-operatingdir \
# 			--disable-speller \
# 			--disable-tabcomp \
# 			--disable-wrapping \
# 			--disable-wrapping-as-root \
# 			--disable-color \
# 			--disable-multibuffer \
# 			--disable-nanorc \
# 			--disable-glibtest \
# 			CC=$(EMBOX_GCC) \
# 	)

# $(CONFIGURE) :
# 	export EMBOX_GCC_LINK=full; \
# 	cd $(PKG_SOURCE_DIR) && ( \
# 		CC=$(EMBOX_GCC) \
# 		CFLAGS="$(EMBOX_IMPORTED_CFLAGS)" \
# 		CPPFLAGS="$(EMBOX_IMPORTED_CPPFLAGS)" \
# 	)
# 	touch $@

# $(BUILD) :
# 	cd $(PKG_SOURCE_DIR) && ( \
# 		$(MAKE) CC=$(EMBOX_GCC) CXX=$(EMBOX_GXX) LD=$(EMBOX_LD) CFLAGS="$(EMBOX_IMPORTED_CFLAGS)" CPPFLAGS="$(EMBOX_IMPORTED_CPPFLAGS)"  WITH_CJSON=no WITH_DOCS=no WITH_TLS=no WITH_TLS_PSK=no make WITH_BUNDLED_DEPS=no \
# 	)

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) CC=$(EMBOX_GCC) CXX=$(EMBOX_GXX) WITH_CJSON=no WITH_DOCS=no WITH_TLS=no WITH_TLS_PSK=no WITH_BUNDLED_DEPS=yes WITH_SHARED_LIBRARIES=no \
	)

# $(BUILD) :
# 	cd $(PKG_SOURCE_DIR) && ( \
# 		$(MAKE) CC=$(arm-none-eabi-gcc) CXX=$(arm-none-eabi-g++) LD=$(arm-none-eabi-ld) CFLAGS="$(EMBOX_IMPORTED_CFLAGS)" CPPFLAGS="$(EMBOX_IMPORTED_CPPFLAGS)"  WITH_CJSON=no WITH_DOCS=no WITH_TLS=no WITH_TLS_PSK=no \
# 	)
# 	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/mosquitto $(PKG_INSTALL_DIR)/mosquitto.o
	touch $@

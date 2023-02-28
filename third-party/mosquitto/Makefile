
PKG_NAME := mosquitto
PKG_VER  := 2.0.15

PKG_SOURCES := https://github.com/eclipse/$(PKG_NAME)/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := 1247e425545affe185278a5314e7bbfe

PKG_PATCHES := pkg_patch.patch

include $(EXTBLD_LIB)

MOSQUITTO_CPPFLAGS = -I$(ROOT_DIR)/third-party/mosquitto/include \
    ${EMBOX_IMPORTED_CPPFLAGS}

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) CC=$(EMBOX_GCC) CXX=$(EMBOX_GXX) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' WITH_CJSON=no WITH_DOCS=no WITH_TLS=no WITH_TLS_PSK=no WITH_BUNDLED_DEPS=yes WITH_SHARED_LIBRARIES=no WITH_STATIC_LIBRARIES=yes WITH_THREADING=no \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/mosquitto $(PKG_INSTALL_DIR)/mosquitto.o
	touch $@

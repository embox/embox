
PKG_NAME := mosquitto
PKG_VER  := 2.0.15

PKG_SOURCES := https://github.com/eclipse/$(PKG_NAME)/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := 1247e425545affe185278a5314e7bbfe

PKG_PATCHES := pkg_patch.patch

include $(EXTBLD_LIB)

MOSQUITTO_CPPFLAGS = "-include $(ROOT_DIR)/third-party/mosquitto/mosquitto_embox_compat.h \
	-I$(ROOT_DIR)/third-party/mosquitto/include \
    ${EMBOX_IMPORTED_CPPFLAGS}"

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) CC=$(EMBOX_GCC) CXX=$(EMBOX_GXX) CPPFLAGS=$(MOSQUITTO_CPPFLAGS) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
	)

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/mosquitto $(PKG_INSTALL_DIR)/mosquitto.o
	touch $@

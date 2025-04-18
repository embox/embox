
PKG_NAME := mosquitto
PKG_VER  := 2.0.15

PKG_SOURCES := https://github.com/eclipse/$(PKG_NAME)/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := 1247e425545affe185278a5314e7bbfe

PKG_PATCHES := pkg_patch.patch

MOSQUITTO_CPPFLAGS = "-include $(ROOT_DIR)/third-party/mosquitto/mosquitto_embox_compat.h \
	-I$(ROOT_DIR)/third-party/mosquitto/include \
	-Dproperty__get_remaining_length=mosquitto_broken_function"

include $(EXTBLD_TARGETS)

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			CC=$(EMBOX_GCC) CXX=$(EMBOX_GXX) CPPFLAGS=$(MOSQUITTO_CPPFLAGS) \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/mosquitto $(PKG_INSTALL_DIR)/mosquitto.o
	cp $(PKG_SOURCE_DIR)/client/mosquitto_pub $(PKG_INSTALL_DIR)/mosquitto_pub.o
	cp $(PKG_SOURCE_DIR)/client/mosquitto_sub $(PKG_INSTALL_DIR)/mosquitto_sub.o
	cp $(PKG_SOURCE_DIR)/lib/libmosquitto.a $(PKG_INSTALL_DIR)/libmosquitto.a
	touch $@

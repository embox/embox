
PKG_NAME := luasocket
PKG_VER  := 2.0.2

PKG_SOURCES := http://files.luaforge.net/releases/$(PKG_NAME)/$(PKG_NAME)/$(PKG_NAME)-$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 41445b138deb7bcfe97bff957503da8e

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			CC=$(EMBOX_GCC) \
			CPPFLAGS='-I$(ROOT_DIR)/third-party/luasocket/include/ \
				$(EMBOX_CPPFLAGS) \
				-DLUA_COMPAT_MODULE' \
	)
	touch $@

$(ROOTFS_IMAGE): $(INSTALL)
$(INSTALL) :
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/mime.so.o $(PKG_INSTALL_DIR)/
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/socket.so.o $(PKG_INSTALL_DIR)/
	mkdir -p $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/*.lua $(ROOT_DIR)/conf/rootfs/socket
	touch $@

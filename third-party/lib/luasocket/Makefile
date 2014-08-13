
PKG_NAME := luasocket
PKG_VER  := d80bb0d82ba105c8fdb27e6174c267965d06ffb0

PKG_SOURCES := https://github.com/diegonehab/luasocket.git
PKG_MD5     := -

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME) && ( \
		make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			LUAV=5.2 \
			DEBUG=DEBUG \
			CC=$(EMBOX_GCC) \
			CPPFLAGS='-I$(ROOT_DIR)/third-party/lib/luasocket/include/ \
				$(EMBOX_CPPFLAGS)' \
	)
	touch $@

$(ROOTFS_IMAGE): $(INSTALL)
$(INSTALL) :
	cp $(BUILD_DIR)/$(PKG_NAME)/src/mime.so.o $(PKG_INSTALL_DIR)/
	cp $(BUILD_DIR)/$(PKG_NAME)/src/socket.so.o $(PKG_INSTALL_DIR)/
	mkdir -p $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(BUILD_DIR)/$(PKG_NAME)/src/*.lua $(ROOT_DIR)/conf/rootfs/socket
	touch $@

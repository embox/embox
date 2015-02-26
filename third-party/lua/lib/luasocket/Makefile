
PKG_NAME := luasocket
PKG_VER  := d80bb0d82ba105c8fdb27e6174c267965d06ffb0

PKG_SOURCES := https://github.com/diegonehab/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5     := da8dd457d3d2b40bb59cf3b0b9f3f667

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			LUAV=5.2 \
			DEBUG=DEBUG \
			CC=$(EMBOX_GCC) \
			CPPFLAGS='-I$(ROOT_DIR)/third-party/lua/lib/luasocket/include/ \
				$(EMBOX_CPPFLAGS)' \
	)
	touch $@

$(ROOTFS_IMAGE) : $(INSTALL)
$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/mime.so.o $(PKG_INSTALL_DIR)/
	cp $(PKG_SOURCE_DIR)/src/socket.so.o $(PKG_INSTALL_DIR)/
	mkdir -p $(ROOT_DIR)/conf/rootfs
	cp -l -f -u $(PKG_SOURCE_DIR)/src/ltn12.lua $(ROOT_DIR)/conf/rootfs
	cp -l -f -u $(PKG_SOURCE_DIR)/src/mime.lua $(ROOT_DIR)/conf/rootfs
	cp -l -f -u $(PKG_SOURCE_DIR)/src/socket.lua $(ROOT_DIR)/conf/rootfs
	mkdir -p $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/ftp.lua $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/headers.lua $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/http.lua $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/mbox.lua $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/smtp.lua $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/tp.lua $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/url.lua $(ROOT_DIR)/conf/rootfs/socket
	touch $@

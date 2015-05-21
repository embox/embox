
PKG_NAME := luasocket
PKG_VER  := 321c0c9b1f7b6b83cd83b58e7e259f53eca69373

PKG_SOURCES := https://github.com/diegonehab/luasocket/archive/$(PKG_VER).tar.gz
PKG_MD5     := e2d6845e1f75d2e24df5563f643598af

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			LUAV=5.3 \
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
	cp -l -f -u $(PKG_SOURCE_DIR)/samples/echosrvr.lua $(ROOT_DIR)/conf/rootfs
	cp -l -f -u $(PKG_SOURCE_DIR)/samples/echoclnt.lua $(ROOT_DIR)/conf/rootfs
	mkdir -p $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/ftp.lua $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/headers.lua $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/http.lua $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/mbox.lua $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/smtp.lua $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/tp.lua $(ROOT_DIR)/conf/rootfs/socket
	cp -l -f -u $(PKG_SOURCE_DIR)/src/url.lua $(ROOT_DIR)/conf/rootfs/socket
	touch $@

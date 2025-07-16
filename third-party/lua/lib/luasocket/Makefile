
PKG_NAME := luasocket
PKG_VER  := 3.1.0

PKG_SOURCES := https://github.com/lunarmodules/luasocket/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := abeca2891168914b57460b0ba9f9090f

PKG_PATCHES := pkg_patch.txt

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			LUAV=5.4 \
			DEBUG=DEBUG \
			CC=$(EMBOX_GCC) \
			CPPFLAGS='-I$(ROOT_DIR)/third-party/lua/lib/luasocket/include/ \
					  -I$(ROOT_DIR)/third-party/lua/lua/include/ \
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


PKG_NAME := lua
PKG_VER  := 5.5.0

PKG_SOURCES := http://www.lua.org/ftp/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := c8b73c52a8c3c80978512f17347ee24d
PKG_PATCHES := pkg_patch.txt

ifneq (0,$(call option_get,BOOLEAN,luasocket_support))
LUA_CPPFLAGS := -DLUASOCKET_SUPPORT
else
LUA_CPPFLAGS :=
endif

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) posix MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="-I$(ROOT_DIR)/third-party/lua/lua/include/ $(LUA_CPPFLAGS)"; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/lua.o $(PKG_INSTALL_DIR)/lua.o
	mkdir -p $(PKG_INSTALL_DIR)/include
	cp $(PKG_SOURCE_DIR)/src/lua.h $(PKG_INSTALL_DIR)/include/
	cp $(PKG_SOURCE_DIR)/src/luaconf.h $(PKG_INSTALL_DIR)/include/
	cp $(PKG_SOURCE_DIR)/src/lauxlib.h $(PKG_INSTALL_DIR)/include/
	touch $@

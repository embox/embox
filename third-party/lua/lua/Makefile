
PKG_NAME := lua
PKG_VER  := 5.4.8

PKG_SOURCES := http://www.lua.org/ftp/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 81cf5265b8634967d8a7480d238168ce
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
			CPPFLAGS="-I$(ROOT_DIR)/third-party/lua/lua/include/ \
				$(LUA_CPPFLAGS)"; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/lua $(PKG_INSTALL_DIR)/lua.o
	mkdir -p $(PKG_INSTALL_DIR)/include
	cp $(PKG_SOURCE_DIR)/src/lua.h $(PKG_INSTALL_DIR)/include/
	cp $(PKG_SOURCE_DIR)/src/luaconf.h $(PKG_INSTALL_DIR)/include/
	cp $(PKG_SOURCE_DIR)/src/lauxlib.h $(PKG_INSTALL_DIR)/include/
	touch $@

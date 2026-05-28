
PKG_NAME := lua
PKG_VER  := 5.5.0

PKG_SOURCES := http://www.lua.org/ftp/$(PKG_NAME)-$(PKG_VER).tar.gz

PKG_MD5_5.5.0      := c8b73c52a8c3c80978512f17347ee24d
PKG_PATCHES_5.5.0  := pkg_patch_5.5.0.txt
LUA_BIN_NAME_5.5.0 := lua.o

PKG_MD5_5.4.8      := 81cf5265b8634967d8a7480d238168ce
PKG_PATCHES_5.4.8  := pkg_patch_5.4.8.txt
LUA_BIN_NAME_5.4.8 := lua

PKG_PATCHES  := $(PKG_PATCHES_$(PKG_VER))
PKG_MD5      := $(PKG_MD5_$(PKG_VER))
LUA_BIN_NAME := $(LUA_BIN_NAME_$(PKG_VER))

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
	cp $(PKG_SOURCE_DIR)/src/$(LUA_BIN_NAME) $(PKG_INSTALL_DIR)/lua.o
	mkdir -p $(PKG_INSTALL_DIR)/include
	cp $(PKG_SOURCE_DIR)/src/lua.h $(PKG_INSTALL_DIR)/include/
	cp $(PKG_SOURCE_DIR)/src/luaconf.h $(PKG_INSTALL_DIR)/include/
	cp $(PKG_SOURCE_DIR)/src/lauxlib.h $(PKG_INSTALL_DIR)/include/
	touch $@

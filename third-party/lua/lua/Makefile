
include $(EXTBLD_DEFINES)

PKG_NAME := lua
PKG_VER  := 5.4.6

PKG_SOURCES := http://www.lua.org/ftp/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 25a429319dff20dfbfb9956c2b5be911
PKG_PATCHES := pkg_patch.txt

ifneq (0,$(call option_get,BOOLEAN,luasocket_support))
LUA_CPPFLAGS := -DLUASOCKET_SUPPORT
else
LUA_CPPFLAGS :=
endif

include $(EXTBLD_TARGETS)

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


PKG_NAME := lua
PKG_VER  := 5.2.3

PKG_SOURCES := http://www.lua.org/ftp/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := dc7f94ec6ff15c985d2d6ad0f1b35654

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

define option_get
$(shell echo OPTION_BOOLEAN_third_party__cmd__lua__$(1) | gcc -P -E -include $(ROOT_DIR)/build/base/src-gen/include/config/third_party/cmd/lua.h -)
endef

LUA_CPPFLAGS :=
ifneq (0,$(call option_get,luasocket_support))
LUA_CPPFLAGS += -DLUASOCKET_SUPPORT
endif

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		make posix MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="-I$(ROOT_DIR)/third-party/lua/include/ \
				$(LUA_CPPFLAGS)"; \
	)
	touch $@

$(INSTALL) :
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/lua $(PKG_INSTALL_DIR)/lua.o
	mkdir -p $(PKG_INSTALL_DIR)/include
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/lua.h $(PKG_INSTALL_DIR)/include/
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/luaconf.h $(PKG_INSTALL_DIR)/include/
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/lauxlib.h $(PKG_INSTALL_DIR)/include/
	touch $@

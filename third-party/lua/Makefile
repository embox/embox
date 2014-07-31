
PKG_NAME := lua
PKG_VER  := 5.2.3

PKG_SOURCES := http://www.lua.org/ftp/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := dc7f94ec6ff15c985d2d6ad0f1b35654

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		make posix MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="-I$(ROOT_DIR)/third-party/lua/include/"; \
	)
	touch $@

$(INSTALL) :
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/lua $(PKG_INSTALL_DIR)/lua.o
	touch $@

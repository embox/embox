
PKG_NAME := luabitop
PKG_VER  := 81bb23b0e737805442033535de8e6d204d0e5381

PKG_SOURCES := https://github.com/LuaDist/luabitop.git
PKG_MD5     := -
PKG_SOURCE_DIR := $(BUILD_DIR)/$(PKG_NAME)

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			CC=$(EMBOX_GCC) \
			CPPFLAGS='$(EMBOX_CPPFLAGS)' \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/bit.so.o $(PKG_INSTALL_DIR)/
	touch $@

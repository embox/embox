
PKG_NAME := mruby
PKG_VER  := 1.1.0

PKG_SOURCES := https://github.com/mruby/mruby/archive/$(PKG_VER).zip
PKG_MD5     := 3897db6c8efc3b946b015489d3ddd1f9

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	# cp $(PKG_SOURCE_DIR)/src/nano $(PKG_INSTALL_DIR)/nano.o
	touch $@

PKG_NAME := linux
PKG_VER := 7750a5aa74f5867336949371f0e18353704d432f

PKG_ARCHIVE_NAME := $(PKG_NAME)-$(PKG_VER).zip
PKG_SOURCES := https://github.com/lkl/linux/archive/$(PKG_VER).zip
PKG_MD5     := 401776073f33d2f179f0ef1a206b9d9f

PKG_PATCHES := lkl-i386-support.patch lkl-embox-support.patch

include $(EXTBLD_LIB)

$(CONFIGURE) :
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR)/tools/lkl && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS) V=1' CC='$(EMBOX_GCC)'; \
	)
	touch $@

$(INSTALL) :
	cd $(PKG_SOURCE_DIR)/tools/lkl && ( \
		cp -r include $(PKG_INSTALL_DIR); \
		cp liblkl.a $(PKG_INSTALL_DIR); \
	)
	touch $@

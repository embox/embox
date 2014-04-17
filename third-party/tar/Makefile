
PKG_NAME := tar
PKG_VER  := 1.27.1

PKG_SOURCES := http://ftp.gnu.org/gnu/tar/$(PKG_NAME)-$(PKG_VER).tar.gz

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CC=$(EMBOX_GCC) \
	)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/tar $(PKG_INSTALL_DIR)/tar.o
	touch $@

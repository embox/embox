
PKG_NAME := tar
PKG_VER  := 1.27.1

PKG_SOURCES := http://ftp.gnu.org/gnu/tar/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 79e59eff05be8d194450d3332522cf03

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CC=$(EMBOX_GCC) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/tar $(PKG_INSTALL_DIR)/tar.o
	touch $@

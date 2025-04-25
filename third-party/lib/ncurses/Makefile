PKG_NAME := ncurses
PKG_VER := 6.3

PKG_SOURCES := https://invisible-mirror.net/archives/ncurses/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5 := a2736befde5fee7d2b7eb45eb281cdbe

PKG_PATCHES := patch.txt

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
	./configure \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--prefix=$(PKG_INSTALL_DIR) \
			--enable-static=yes \
			--without-cxx \
			CC=$(EMBOX_GCC) \
			CXX=$(EMBOX_GXX) \
	)
	touch $@

$(BUILD) :
	cp comp_captab.c $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/ncurses/comp_captab.c; \
	cp comp_userdefs.c $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/ncurses/comp_userdefs.c; \
	echo "all:" > $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/test/Makefile
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	echo "install:" > $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/test/Makefile ;
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		$(MAKE) install MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' ; \
		cp -r $(PKG_INSTALL_DIR)/include/ncurses $(INCLUDE_INSTALL_DIR) ; \
	)
	touch $@

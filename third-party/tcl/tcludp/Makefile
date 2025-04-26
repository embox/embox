
PKG_NAME := tcludp
PKG_VER  := 1.0.10

PKG_SOURCES := http://sourceforge.net/projects/tcludp/files/$(PKG_NAME)/$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz \
	http://download.embox.rocks/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 1843398b4ee9fbc17eb2e19465710f80

PKG_PATCHES := pkg_patch.txt

tcl_cppflags = -include $(ROOT_DIR)/third-party/tcl/tcludp/tcludp_embox_compat.h

$(CONFIGURE) :
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="$(tcl_cppflags)" \
			--disable-shared \
			--disable-load \
			--disable-threads \
			--enable-symbols \
			--with-tcl=$(ROOT_DIR)/build/extbld/third_party/tcl/core/tcl8.6.1/unix \
			--with-tclinclude=$(ROOT_DIR)/build/extbld/third_party/tcl/core/install/include \
	)
	touch $@

$(BUILD) :
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/libudp1.0.10.a $(PKG_INSTALL_DIR)/libudp1.0.10.a
	touch $@

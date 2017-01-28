PKG_NAME := samba
PKG_VER  := 3.6.18

PKG_SOURCES := http://ftp.samba.org/pub/samba/stable/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := c7eec3e83fe4c4750240a8a0a214bbd4

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

samba_cppflags = -I$(ROOT_DIR)/third-party/samba_ext/include

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/source3 && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="$(samba_cppflags)" \
			--with-libiconv=$(PKG_INSTALL_DIR)\
			--enable-debug \
			--enable-developer \
			--disable-largefile \
			--disable-pie \
			--enable-static=ALL \
			--disable-shared \
			--disable-shared-libs \
			samba_cv_CC_NEGATIVE_ENUM_VALUES=yes \
			ac_cv_file__proc_sys_kernel_core_pattern=no \
		)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/source3 && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/source3/bin/smbclient $(PKG_INSTALL_DIR)/smbclient.o
	touch $@

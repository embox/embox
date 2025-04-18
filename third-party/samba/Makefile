PKG_NAME := samba
PKG_VER  := 3.6.25

PKG_SOURCES := http://ftp.samba.org/pub/samba/stable/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 76da2fa64edd94a0188531e7ecb27c4e

PKG_PATCHES := patch.txt

samba_cppflags = -include $(ROOT_DIR)/third-party/samba/samba_embox_compat.h \
        -I$(ROOT_DIR)/third-party/samba/include \
        -I$(ROOT_DIR)/build/extbld/third_party/zlib/libs/zlib-1.2.8

include $(EXTBLD_TARGETS)

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
			ac_cv_header_zlib_h=yes \
			ac_cv_lib_ext_z_zlibVersion=yes \
			samba_cv_zlib_1_2_3=yes \
		)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/source3 && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/source3/bin/smbclient $(PKG_INSTALL_DIR)/smbclient.embox.o
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/source3/bin/libsmbclient.a $(PKG_INSTALL_DIR)/libsmbclient.a
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/source3/bin/libtevent.a $(PKG_INSTALL_DIR)/libtevent.a
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/source3/bin/libtalloc.a $(PKG_INSTALL_DIR)/libtalloc.a
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/source3/bin/libtdb.a $(PKG_INSTALL_DIR)/libtdb.a
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/source3/bin/libwbclient.a $(PKG_INSTALL_DIR)/libwbclient.a
	touch $@

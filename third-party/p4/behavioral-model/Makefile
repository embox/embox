PKG_NAME := behavioral-model
PKG_VER := 1.15.0

PKG_SOURCES := https://github.com/p4lang/$(PKG_NAME)/archive/refs/tags/$(PKG_VER).tar.gz

PKG_MD5     := dc9d34e33daf40e54d9d71a69aece2dc
# PKG_PATCHES := timeradd_timersub.patch
# PKG_PATCHES += fix_linux_types_place.patch
# PKG_PATCHES += change_define_linux.patch
# PKG_PATCHES += add_types_define.patch

# LIBPCAP_CFLAGS = -I$(ROOT_DIR)/third-party/lib/libpcap/include
# LIBPCAP_CFLAGS = -I$(ROOT_DIR)/src/posix/include

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ./autogen.sh && ( \
		./configure --prefix=$(PKG_INSTALL_DIR)  \
			CC=$(EMBOX_GCC) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

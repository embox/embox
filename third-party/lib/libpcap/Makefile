PKG_NAME := libpcap
PKG_VER := 1.10.1

#PKG_SOURCES := git@github.com:the-tcpdump-group/libpcap.git
PKG_SOURCES := https://github.com/the-tcpdump-group/libpcap/archive/refs/tags/$(PKG_NAME)-$(PKG_VER).tar.gz

PKG_MD5     := 81fc402b01ccacac7fff08518c4458ec
PKG_PATCHES := timeradd_timersub.patch
PKG_PATCHES += fix_linux_types_place.patch
PKG_PATCHES += change_define_linux.patch
PKG_PATCHES += add_types_define.patch

LIBPCAP_CFLAGS = -I$(ROOT_DIR)/third-party/lib/libpcap/include
# LIBPCAP_CFLAGS = -I$(ROOT_DIR)/src/posix/include

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure --prefix=$(PKG_INSTALL_DIR)  \
			CC=$(EMBOX_GCC) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--with-pcap=linux \
			CFLAGS="$(LIBPCAP_CFLAGS)" \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) install MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

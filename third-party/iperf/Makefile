
PKG_NAME := iperf
PKG_VER  := 2.1.9

PKG_SOURCES := https://sourceforge.net/projects/iperf2/files/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := e6f8ca3ff2d6911da24ac3f1a319b799

PKG_PATCHES := pkg_patch.txt

IPERF_CPPFLAGS := "-I$(ROOT_DIR)/third-party/iperf/include"

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--enable-debuginfo \
			--enable-static-bin \
			CC=$(EMBOX_GCC) \
			CXX=$(EMBOX_GXX) \
			CPPFLAGS=$(IPERF_CPPFLAGS) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/iperf $(PKG_INSTALL_DIR)/iperf.o
	touch $@

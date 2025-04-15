
PKG_NAME := iperf
PKG_VER  := 3.13

PKG_SOURCES := https://github.com/esnet/$(PKG_NAME)/archive/refs/tags/$(PKG_VER).tar.gz
PKG_MD5     := 43c734bf63157ec277d706879dc3b2c0

PKG_PATCHES := pkg_patch.txt

IPERF3_CPPFLAGS := -I$(ROOT_DIR)/third-party/iperf3/include

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--enable-static-bin \
			CC=$(EMBOX_GCC) \
			CPPFLAGS=$(IPERF3_CPPFLAGS) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR)/src && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/iperf3 $(PKG_INSTALL_DIR)/iperf3.o
	touch $@

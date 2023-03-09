
PKG_NAME := iperf
PKG_VER  := 2.1.8

PKG_SOURCES := https://sourceforge.net/projects/iperf2/files/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 52d38af7208c64755066a9d4fd970360

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CC=$(EMBOX_GCC) CXX=$(EMBOX_GXX) \
			CPPFLAGS=-DIP_TTL=1 \
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

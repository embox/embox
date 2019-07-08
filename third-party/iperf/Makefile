
PKG_NAME := iperf
PKG_VER  := 2.0.5

PKG_SOURCES := http://citylan.dl.sourceforge.net/project/iperf/$(PKG_NAME)-$(PKG_VER).tar.gz \
	http://download.embox.rocks/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 44b5536b67719f4250faed632a3cd016

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--disable-shared \
			--disable-static \
			CC=$(EMBOX_GCC) CXX=$(EMBOX_GXX) \
			CPPFLAGS="-I$(ROOT_DIR)/third-party/iperf/include/ \
				-Drpl_malloc=malloc \
				-Ulinux"; \
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

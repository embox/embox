
PKG_NAME := httpd
PKG_VER  := 2.4.10

PKG_SOURCES := http://apache-mirror.rbc.ru/pub/apache/$(PKG_NAME)/$(PKG_NAME)-$(PKG_VER).tar.bz2
PKG_MD5     := 44543dff14a4ebc1e9e2d86780507156

#PKG_PATCHES :=

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CC=$(EMBOX_GCC) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/httpd $(PKG_INSTALL_DIR)/httpd.o
	touch $@

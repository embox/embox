PKG_NAME := libcoap
PKG_VER  := 4.1.1

PKG_SOURCES := https://github.com/obgm/$(PKG_NAME)/archive/v$(PKG_VER).tar.gz
PKG_MD5     := ff1af4bd9893c1728e765aa08a816e41

include $(EXTBLD_LIB)

$(CONFIGURE) :
	cd $(PKG_SOURCE_DIR) && autoconf && ( \
		./configure CC=$(EMBOX_GCC) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--enable-documentation=no \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		make; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/libcoap.a $(PKG_INSTALL_DIR)/
	cp $(PKG_SOURCE_DIR)/*.h $(PKG_INSTALL_DIR)/
	touch $@

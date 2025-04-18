PKG_NAME := libcoap
PKG_VER  := 4.1.1

PKG_SOURCES := https://github.com/obgm/$(PKG_NAME)/archive/v$(PKG_VER).tar.gz
PKG_MD5     := ff1af4bd9893c1728e765aa08a816e41

coap_cppflags = -include $(ROOT_DIR)/third-party/libcoap/lib/libcoap_embox_compat.h \
			-Drpl_malloc=malloc

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && autoconf && ( \
		./configure \
			CC=$(EMBOX_GCC) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CPPFLAGS="$(coap_cppflags)" \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) \
                	CC=$(EMBOX_GCC) \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/libcoap.a $(PKG_INSTALL_DIR)/
	cp $(PKG_SOURCE_DIR)/*.h $(PKG_INSTALL_DIR)/
	cp $(PKG_SOURCE_DIR)/examples/server.o $(PKG_INSTALL_DIR)/../../coap_server/install/
	cp $(PKG_SOURCE_DIR)/examples/client.o $(PKG_INSTALL_DIR)/../../coap_client/install/
	touch $@

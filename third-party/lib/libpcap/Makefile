PKG_SOURCES := git@github.com:the-tcpdump-group/libpcap.git

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure --prefix=$(PKG_INSTALL_DIR)  \
			CC=$(EMBOX_GCC) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			$(DISABLE_FEATURES:%=--disable-%) \
	)
	touch $@

$(BUILD) :
#	cd $(PKG_SOURCE_DIR) && ( \
#		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
#	)
	touch $@

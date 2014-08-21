
PKG_NAME := pjproject
PKG_VER  := 2.2.1

PKG_SOURCES := http://www.pjsip.org/release/$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.bz2
PKG_MD5     := 6ed4bb7750c827dc1d881e209a3b62db

include $(EXTBLD_LIB)

PKG_PATCHES := pjproject.patch cxx_builds_disable.patch

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CC=$(EMBOX_GCC) \
			--disable-sound; \
		echo export CFLAGS+=$(EMBOX_DEPS_CPPFLAGS) > user.mak; \
		echo export CXXFLAGS+=$(EMBOX_DEPS_CPPFLAGS) >> user.mak; \
	)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		make -j1 dep; \
		make -j1 MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	for f in $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/pjsip-apps/bin/samples/$(AUTOCONF_TARGET_TRIPLET)/*; do \
       		cp $$f $(PKG_INSTALL_DIR)/$$(basename $$f).o; \
	done
	touch $@

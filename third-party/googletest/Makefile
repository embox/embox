PKG_NAME := googletest
PKG_VER  := 1.17.0

PKG_SOURCES := https://github.com/google/$(PKG_NAME)/releases/download/v$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5 := b6f100bc2a5853a48046aa168ececf84

$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	$(MKDIR) $(PKG_INSTALL_DIR)/src
	cd $(PKG_SOURCE_DIR) && ( \
		cp -r * $(PKG_INSTALL_DIR)/src/ ; \
	)
	touch $@

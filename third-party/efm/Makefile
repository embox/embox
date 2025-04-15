PKG_NAME := efm
PKG_VER  := ed4a16901b347762c2a2a21eba2f84d06317ed07

PKG_SOURCES := https://github.com/embox/efm/archive/$(PKG_VER).zip
PKG_MD5     := edd02177de489eb969fa49791d118361

include $(EXTBLD_TARGETS)

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) CC=$(EMBOX_GCC) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/$(PKG_NAME) $(PKG_INSTALL_DIR)/$(PKG_NAME).o
	touch $@

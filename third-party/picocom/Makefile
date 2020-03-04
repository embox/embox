
PKG_NAME := picocom
PKG_VER  := master

PKG_SOURCES := http://github.com/npat-efault/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5     := eb4836c3db7e1098c1fd563ee87fa170

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	cp impl.h $(PKG_SOURCE_DIR)/impl.h
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) CC=$(EMBOX_GCC) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/picocom $(PKG_INSTALL_DIR)/picocom.o
	touch $@

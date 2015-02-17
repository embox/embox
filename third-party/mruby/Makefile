
PKG_NAME := mruby
PKG_VER  := 1.1.0

PKG_SOURCES := https://github.com/mruby/mruby/archive/$(PKG_VER).zip
PKG_MD5     := 3897db6c8efc3b946b015489d3ddd1f9

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	cp -f $(THIRDPARTY_DIR)/mruby/build_config.rb $(PKG_SOURCE_DIR)/
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		make CROSS_CC=$(EMBOX_GCC) \
		CROSS_LD=$(EMBOX_CROSS_COMPILE)ld \
		CROSS_AR=$(EMBOX_CROSS_COMPILE)ar \
		CROSS_LDFLAGS='$(EMBOX_LDFLAGS) -r' \
		CROSS_CFLAGS="-I$(ROOT_DIR)/build/extbld/third_party/cmd/mruby/mruby-1.1.0/include/"; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/build/embox/bin/mrbc $(PKG_INSTALL_DIR)/mrbc.o
	touch $@


PKG_NAME := mruby
#PKG_VER  := 1.1.0
PKG_VER  := 3.2.0

#PKG_SOURCES := https://github.com/mruby/mruby/archive/$(PKG_VER).tar.gz
#PKG_MD5     := 60d75ea704c9285f3c80b3ad1d2b68de
PKG_SOURCES := https://github.com/mruby/mruby/archive/refs/tags/$(PKG_VER).tar.gz
PKG_MD5     := fbb67cc8a1ecfb9cdc1307ec1c02b489

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	cp -f $(THIRDPARTY_DIR)/mruby/build_config.rb $(PKG_SOURCE_DIR)/build_config/default.rb
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) CROSS_CC=$(EMBOX_GCC) \
		CROSS_LD=$(EMBOX_CROSS_COMPILE)ld \
		CROSS_AR=$(EMBOX_CROSS_COMPILE)ar \
		CROSS_LDFLAGS='$(EMBOX_LDFLAGS) -r' \
		CROSS_CFLAGS="-I$(ROOT_DIR)/build/extbld/third_party/cmd/mruby/mruby-3.2.0/include/"; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/build/embox/bin/mirb $(PKG_INSTALL_DIR)/mibc.o
	cp $(PKG_SOURCE_DIR)/build/embox/bin/mruby $(PKG_INSTALL_DIR)/mruby.o
	touch $@


PKG_NAME := STLport
PKG_VER  := 5.2.1

PKG_SOURCES := https://netcologne.dl.sourceforge.net/project/stlport/STLport/$(PKG_NAME)-$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.bz2 \
	http://download.embox.rocks/$(PKG_NAME)-$(PKG_VER).tar.bz2

PKG_MD5     := a8341363e44d9d06a60e03215b38ddde

PKG_PATCHES := patch.txt

STLPORT_CPPFLAGS += -I../../stlport
STLPORT_CPPFLAGS += -DUSE_SPRINTF_INSTEAD
STLPORT_CPPFLAGS += -I$(abspath .)/include
STLPORT_CPPFLAGS += -Wno-narrowing

ifneq (,$(filter -fno-rtti,$(EMBOX_CXXFLAGS)))
EMBOX_RTTI_FLAG          = --without-rtti
else
EMBOX_RTTI_FLAG          =
endif

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure \
			--target=embox \
			--with-cc=$(EMBOX_GCC) \
			--with-cxx=$(EMBOX_GXX) \
			--with-extra-cflags="$(STLPORT_CPPFLAGS)" \
			--with-extra-cxxflags="$(STLPORT_CPPFLAGS)" \
			--enable-static \
			--disable-shared \
			$(EMBOX_RTTI_FLAG) \
			--with-static-rtl \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	mkdir -p $(PKG_INSTALL_DIR)/lib/ $(PKG_INSTALL_DIR)/include/
	cp $(PKG_SOURCE_DIR)/build/lib/obj/embox-gcc/so_g/libstlportg.a $(PKG_INSTALL_DIR)/lib/
	cp -r -f $(PKG_SOURCE_DIR)/stlport $(PKG_INSTALL_DIR)/include/
	touch $@

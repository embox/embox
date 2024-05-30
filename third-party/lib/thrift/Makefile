PKG_NAME := thrift
PKG_VER := 0.11.0

PKG_SOURCES := https://archive.apache.org/dist/thrift/0.11.0/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_PATCHES := patches/fix.patch
PKG_MD5     := 0be59730ebce071eceaf6bfdb8d3a20e

THRIFT_CXXFLAGS = -I$(ROOT_DIR)/build/extbld/third_party/boost/libs/install/include
THRIFT_CXXFLAGS += -I$(ROOT_DIR)/build/extbld/third_party/lib/openssl/install/include

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure --prefix=$(PKG_INSTALL_DIR)  \
			CC=$(EMBOX_GCC) \
			CFLAGS="$(THRIFT_CXXFLAGS)" \
			CXX=$(EMBOX_GXX) \
			CPPFLAGS="$(THRIFT_CXXFLAGS)" \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--with-boost="$(ROOT_DIR)/build/extbld/third_party/boost/libs/install/include" \
			--with-boost-libdir="$(ROOT_DIR)/build/extbld/third_party/boost/libs/install/lib" \
			--with-openssl="$(ROOT_DIR)/build/extbld/third_party/lib/openssl/install/lib" \
			--without-csharp \
			--without-go \
			--without-nodejs \
			--without-python \
			--without-dotnetcore \
			--enable-static=yes \
			--enable-tests=no \
			--enable-tutorial=no \
			--without-c_glib \
	)
	touch $@

$(BUILD) :
	cp $(ROOT_DIR)/third-party/lib/thrift/patches/config.h $(PKG_SOURCE_DIR)/lib/cpp/src/thrift/config.h
	cp $(ROOT_DIR)/third-party/lib/thrift/patches/config.h $(PKG_SOURCE_DIR)/config.h
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) install MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

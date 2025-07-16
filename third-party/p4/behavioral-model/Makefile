PKG_NAME := behavioral-model
PKG_VER := 1.15.0

PKG_SOURCES := https://github.com/p4lang/$(PKG_NAME)/archive/refs/tags/$(PKG_VER).tar.gz

PKG_MD5     := dc9d34e33daf40e54d9d71a69aece2dc

PKG_PATCHES := only_l2_switch.patch
PKG_PATCHES += disable_lib_check.patch
PKG_PATCHES += fix.patch

# BM_CXXFLAGS = -include $(ROOT_DIR)/third-party/p4/behavioral-model/bmv2_embox_compat.h
BM_CXXFLAGS = -I$(ROOT_DIR)/build/extbld/third_party/lib/libpcap/install/include
BM_CXXFLAGS += -I$(ROOT_DIR)/build/extbld/third_party/lib/libgmp/install/include
BM_CXXFLAGS += -I$(ROOT_DIR)/build/extbld/third_party/lib/thrift/install/include
BM_CXXFLAGS += -I$(ROOT_DIR)/build/extbld/third_party/lib/openssl/install/include
BM_CXXFLAGS += -I$(ROOT_DIR)/build/extbld/third_party/boost/libs/install/include
# BM_LDFLAGS = -L$(ROOT_DIR)/build/extbld/third_party/lib/libgmp/install/lib
# BM_CXXFLAGS += -I/build/extbld/third_party/gcc/gcc_build/install/_target/include/c++/_gcc_version/

# LIBPCAP_CFLAGS = -I$(ROOT_DIR)/src/posix/include

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ./autogen.sh && ( \
		./configure --prefix=$(PKG_INSTALL_DIR)  \
			CC=$(EMBOX_GCC) \
			CFLAGS="$(BM_CXXFLAGS)" \
			CXX=$(EMBOX_GXX) \
			CXXFLAGS="$(BM_CXXFLAGS)" \
			LDFLAGS=$(BM_LDFLAGS) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--with-thrift=$(ROOT_DIR)/build/extbld/third_party/lib/thrift/install/include \
			--with-nanomsg=no \
			--enable-static \
			--disable-shared \
			--disable-undeterministic-tests \
			--with-stress-tests=no \
	)
	touch $@

$(BUILD) :
	cp $(ROOT_DIR)/third-party/p4/behavioral-model/patches/config.h $(PKG_SOURCE_DIR)/config.h
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/targets/l2_switch/l2_switch $(PKG_INSTALL_DIR)/l2_switch.o
	touch $@

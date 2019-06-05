
PKG_NAME := gcc

#PKG_VER  := 4.8.1
PKG_VER  := 6.3.0

PKG_SOURCES := http://ftp.gnu.org/gnu/gcc/$(PKG_NAME)-$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz

#PKG_MD5     := 74cc12b7afe051ab7d0e00269e49fc9b
PKG_MD5 := 6e5ea04789678f1250c1b30c4d9ec417

#PKG_PATCHES := pkg_patch.txt
PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

ifeq (arm,$(EMBOX_ARCH))
AUTOCONF_TARGET_TRIPLET := arm-none-eabi
else ifeq (sparc,$(EMBOX_ARCH))
AUTOCONF_TARGET_TRIPLET := sparc-elf
else ifeq (mips,$(EMBOX_ARCH))
AUTOCONF_TARGET_TRIPLET := mips-elf
else ifeq (x86,$(EMBOX_ARCH))
AUTOCONF_TARGET_TRIPLET := i386-pc-elf
else
$(error Unsupported architecture: $(EMBOX_ARCH))
endif

target_libgcc_cppflags := -DLIBGCC2_HAS_XF_MODE=0 \
	-DDO_GLOBAL_CTORS_BODY -DDO_GLOBAL_DTORS_BODY \
	-DNAME__MAIN -DSYMBOL__MAIN=__embox_register_eh_frame_once
cppflags = -I$(ROOT_DIR)/third-party/gcc/include \
	$${EMBOX_IMPORTED_CPPFLAGS} \
	-I$(PKG_BUILD_DIR)/gcc/include
PKG_BUILD_DIR  := $(BUILD_DIR)/build

EMBOX_DERIVED_CXXFLAGS = $(filter -g% -f% -m% -O% -G% -E% -Wa%,$(EMBOX_CXXFLAGS))
ifeq (arm,$(EMBOX_ARCH))
GCC_IMPORTED_CXXFLAGS = $(filter-out -std=% -fno-rtti,$(EMBOX_DERIVED_CXXFLAGS))
else
GCC_IMPORTED_CXXFLAGS = $(filter-out -std=%, $(EMBOX_DERIVED_CXXFLAGS))
endif

$(CONFIGURE) :
	mkdir -p $(PKG_BUILD_DIR) && cd $(PKG_BUILD_DIR) && ( \
		. $(EMBOX_GCC_ENV) && $(PKG_SOURCE_DIR)/configure \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--with-cross-host=embox \
			--disable-multilib \
			--disable-shared \
			--disable-tls \
			--disable-nls \
			--disable-libssp \
			--without-headers \
			--without-newlib \
			--enable-languages=c,c++ \
			--enable-soft-float \
			CC_FOR_TARGET=$${EMBOX_CROSS_COMPILE}gcc \
			CXX_FOR_TARGET=$${EMBOX_CROSS_COMPILE}g++ \
			CFLAGS_FOR_TARGET="$(cppflags) $(target_libgcc_cppflags) $${EMBOX_IMPORTED_CFLAGS}" \
			CXXFLAGS_FOR_TARGET="$(cppflags) $(GCC_IMPORTED_CXXFLAGS) -fpermissive" \
			LDFLAGS_FOR_TARGET="$${EMBOX_IMPORTED_LDFLAGS}"; \
	)
	touch $@

$(BUILD) :
	cd $(PKG_BUILD_DIR) && ( \
		. $(EMBOX_GCC_ENV) && $(MAKE) MAKEFLAGS='$${EMBOX_IMPORTED_MAKEFLAGS}' \
			CC_FOR_TARGET=$${EMBOX_CROSS_COMPILE}gcc \
			GCC_FOR_TARGET=$${EMBOX_CROSS_COMPILE}gcc \
			CXX_FOR_TARGET=$${EMBOX_CROSS_COMPILE}g++ \
			RAW_CXX_FOR_TARGET=$${EMBOX_CROSS_COMPILE}g++ \
			AR_FOR_TARGET=ar \
			RANLIB_FOR_TARGET=ranlib \
			TARGET-libiberty="LINKER=true CC=true AR=true RANLIB=touch all" \
			TARGET-zlib="LINKER=true CC=true AR=true RANLIB=touch all" \
			TARGET-libbacktrace="LINKER=true CC=true AR=touch \
				LINK=\"mkdir -p .libs && true\" \
				LIBS=\"&& touch .libs/libbacktrace.a\" all" \
			TARGET-libcpp="LINKER=true CC=true CXX=true POSTCOMPILE=true \
				AR=true RANLIB=touch all" \
			TARGET-libdecnumber="LINKER=true CC=true AR=true RANLIB=touch all" \
			TARGET-fixincludes="LINKER=true CC=true AR=true all" \
			TARGET-lto-plugin="CC=true LIBTOOL=true all" \
			TARGET-gcc="LINKER=true CC=true AR=true \
				GCC_FOR_TARGET=$${EMBOX_CROSS_COMPILE}gcc \
				AR=true RANLIB=true libgcc-support stmp-int-hdrs CXXDEPMODE=true" \
			TARGET-target-libgcc="PICFLAG= EXTRA_PARTS= all" \
			all-target-libgcc all-target-libstdc++-v3; \
	)
	touch $@

STL_HEADERS := algorithm bitset complex forward_list future iosfwd iterator locale \
	mutex queue regex shared_mutex stdexcept system_error typeindex unordered_set vector \
	array chrono condition_variable fstream iomanip iostream limits map numeric random \
	scoped_allocator sstream streambuf thread type_traits utility atomic codecvt deque \
	functional ios istream list memory ostream ratio set stack string tuple unordered_map valarray

$(INSTALL) :
	$(eval BITS_HEADERS := $(wildcard $(PKG_BUILD_DIR)/$(AUTOCONF_TARGET_TRIPLET)/libstdc++-v3/include/bits/*))
	$(eval BITS_HEADERS := $(filter-out $(PKG_BUILD_DIR)/$(AUTOCONF_TARGET_TRIPLET)/libstdc++-v3/include/bits/stamp-bits, $(BITS_HEADERS)))
	$(eval BITS_TARGET_HEADERS := $(wildcard $(PKG_BUILD_DIR)/$(AUTOCONF_TARGET_TRIPLET)/libstdc++-v3/include/$(AUTOCONF_TARGET_TRIPLET)/bits/*))
	$(eval EXT_HEADERS := $(wildcard $(PKG_BUILD_DIR)/$(AUTOCONF_TARGET_TRIPLET)/libstdc++-v3/include/ext/*))
	$(eval DEBUG_HEADERS := $(wildcard $(PKG_BUILD_DIR)/$(AUTOCONF_TARGET_TRIPLET)/libstdc++-v3/include/debug/*))
	$(eval BACKWARD_HEADERS := $(wildcard $(PKG_BUILD_DIR)/$(AUTOCONF_TARGET_TRIPLET)/libstdc++-v3/include/backward/*))

	mkdir -p $(PKG_INSTALL_DIR)/include $(PKG_INSTALL_DIR)/include/bits
	mkdir -p $(PKG_INSTALL_DIR)/include/std $(PKG_INSTALL_DIR)/include/ext
	mkdir -p $(PKG_INSTALL_DIR)/include/debug $(PKG_INSTALL_DIR)/include/backward
	cp $(PKG_BUILD_DIR)/$(AUTOCONF_TARGET_TRIPLET)/libgcc/unwind.h $(PKG_INSTALL_DIR)/include/
	cp $(PKG_SOURCE_DIR)/libstdc++-v3/libsupc++/cxxabi.h $(PKG_INSTALL_DIR)/include/
	cp $(PKG_SOURCE_DIR)/libstdc++-v3/libsupc++/exception $(PKG_INSTALL_DIR)/include/
	cp $(PKG_SOURCE_DIR)/libstdc++-v3/libsupc++/initializer_list $(PKG_INSTALL_DIR)/include/
	cp $(PKG_SOURCE_DIR)/libstdc++-v3/libsupc++/new $(PKG_INSTALL_DIR)/include/
	cp $(PKG_SOURCE_DIR)/libstdc++-v3/libsupc++/typeinfo $(PKG_INSTALL_DIR)/include/
	cp $(PKG_SOURCE_DIR)/libstdc++-v3/libsupc++/atomic_lockfree_defines.h $(PKG_INSTALL_DIR)/include/bits/
	cp -r $(BITS_HEADERS) $(PKG_INSTALL_DIR)/include/bits/
	cp -r $(BITS_TARGET_HEADERS) $(PKG_INSTALL_DIR)/include/bits/
	cp -r $(EXT_HEADERS) $(PKG_INSTALL_DIR)/include/ext/
	cp -r $(DEBUG_HEADERS) $(PKG_INSTALL_DIR)/include/debug/
	cp -r $(BACKWARD_HEADERS) $(PKG_INSTALL_DIR)/include/backward/
	cd $(PKG_BUILD_DIR)/$(AUTOCONF_TARGET_TRIPLET)/libstdc++-v3/include/; cp $(STL_HEADERS) $(PKG_INSTALL_DIR)/include/std/; cd ..; \
	mkdir -p $(PKG_INSTALL_DIR)/lib
	cp $(PKG_BUILD_DIR)/$(AUTOCONF_TARGET_TRIPLET)/libgcc/libgcc.a $(PKG_INSTALL_DIR)/lib/
	cp $(PKG_BUILD_DIR)/$(AUTOCONF_TARGET_TRIPLET)/libstdc++-v3/libsupc++/.libs/libsupc++.a $(PKG_INSTALL_DIR)/lib
	cp $(PKG_BUILD_DIR)/$(AUTOCONF_TARGET_TRIPLET)/libstdc++-v3/src/.libs/libstdc++.a $(PKG_INSTALL_DIR)/lib
	touch $@

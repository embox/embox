#
# We maintain several GCC versions because if you have, for instance, gcc7
# you will not be able to build gcc9. This happens because gcc9 (or gcc8)
# use some new features during own build that wasn't supported in gcc7 yet,
# and we do not bootstrap GCC, only build libgcc, libsupc++, and libstdc++.
# We intentionaly skip these steps because of build time optimization,
# which leads to these consequences: gcc9 cannot be built with gcc7.
#
# On the other hand, because of "Thread mode: single", we cannot just take
# libstdc++ for the toolchain.
#
# I see several possible solutions:
#   1. The current one - i.e. pass desired GCC version as an option.
#   2. Get rid of build time optimization (CC=true AR=true below).
#      Build will take longer time, but in theory you can build it with
#      any toolchain.
#   3. Or, build Embox's own toolchain which supports libgcc,
#      libsupc++, and libstdc++. With "Thread mode: posix".
#

define option_get_string
$(shell echo OPTION_STRING_third_party__gcc__gcc_build__$(1) | gcc -P -E -include $(SRCGEN_DIR)/include/config/third_party/gcc/gcc_build.h -)
endef

PKG_NAME := gcc

PKG_VER:=$(call option_get_string,gcc_version)

PKG_SOURCES := http://ftp.gnu.org/gnu/gcc/$(PKG_NAME)-$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz

PKG_MD5_6.3.0  := 6e5ea04789678f1250c1b30c4d9ec417
PKG_MD5_8.3.0  := 9972f8c24c02ebcb5a342c1b30de69ff
PKG_MD5_9.3.0  := 9b7e8f6cfad96114e726c752935af58a
PKG_MD5_13.2.0 := aeb5ac806c34d47e725bdd025f34bac4

PKG_MD5:=$(PKG_MD5_$(PKG_VER))

PKG_PATCHES := patch_$(PKG_VER).txt

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
	-DDO_GLOBAL_CTORS_BODY -DDO_GLOBAL_DTORS_BODY

cppflags = $${EMBOX_IMPORTED_CPPFLAGS} \
	-I$(PKG_BUILD_DIR)/gcc/include

EMBOX_DERIVED_CXXFLAGS = $(filter -g% -f% -m% -O% -G% -E% -Wa%,$(EMBOX_CXXFLAGS))
ifeq (arm,$(EMBOX_ARCH))
GCC_IMPORTED_CXXFLAGS = $(filter-out -std=% -fno-rtti,$(EMBOX_DERIVED_CXXFLAGS))
else
GCC_IMPORTED_CXXFLAGS = $(filter-out -std=%, $(EMBOX_DERIVED_CXXFLAGS))
endif

PKG_BUILD_DIR  := $(BUILD_DIR)/build

$(CONFIGURE) :
	mkdir -p $(PKG_BUILD_DIR) && cd $(PKG_BUILD_DIR) && ( \
		. $(EMBOX_GCC_ENV) && $(PKG_SOURCE_DIR)/configure \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--with-cross-host=embox \
			--prefix=$(PKG_INSTALL_DIR) \
			--disable-multilib \
			--disable-shared \
			--disable-tls \
			--disable-nls \
			--disable-decimal-float \
			--disable-libffi \
			--disable-libgomp \
			--disable-libmudflap \
			--disable-libquadmath \
			--disable-libstdcxx-pch \
			--disable-libssp \
			--disable-bootstrap \
			--without-headers \
			--without-newlib \
			--enable-languages=c,c++ \
			--enable-soft-float \
			--enable-hosted-libstdcxx \
			CC_FOR_TARGET=$${EMBOX_CROSS_COMPILE}gcc \
			CXX_FOR_TARGET=$${EMBOX_CROSS_COMPILE}g++ \
			CFLAGS_FOR_TARGET="$(cppflags) $(target_libgcc_cppflags) $${EMBOX_IMPORTED_CFLAGS}" \
			CXXFLAGS_FOR_TARGET="$(cppflags) $(GCC_IMPORTED_CXXFLAGS) -fpermissive" \
			LDFLAGS_FOR_TARGET="$${EMBOX_IMPORTED_LDFLAGS}"; \
	)
	touch $@

$(BUILD) :
	cd $(PKG_BUILD_DIR) && ( \
		. $(EMBOX_GCC_ENV) && $(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			CC_FOR_TARGET=$${EMBOX_CROSS_COMPILE}gcc \
			GCC_FOR_TARGET=$${EMBOX_CROSS_COMPILE}gcc \
			CXX_FOR_TARGET=$${EMBOX_CROSS_COMPILE}g++ \
			RAW_CXX_FOR_TARGET=$${EMBOX_CROSS_COMPILE}g++ \
			AR_FOR_TARGET=$${EMBOX_CROSS_COMPILE}ar \
			RANLIB_FOR_TARGET=$${EMBOX_CROSS_COMPILE}ranlib \
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

$(INSTALL) :
	cd $(PKG_BUILD_DIR) && ( \
		. $(EMBOX_GCC_ENV) && \
		$(MAKE) install-target-libgcc RANLIB_FOR_TARGET=$${EMBOX_CROSS_COMPILE}ranlib; \
		$(MAKE) install-target-libstdc++-v3 RANLIB_FOR_TARGET=$${EMBOX_CROSS_COMPILE}ranlib; \
	)
	mkdir -p $(PKG_INSTALL_DIR)/libs
	cp $(PKG_INSTALL_DIR)/lib/gcc/$(AUTOCONF_TARGET_TRIPLET)/$(PKG_VER)/libgcc.a $(PKG_INSTALL_DIR)/libs
	cp $(PKG_INSTALL_DIR)/$(AUTOCONF_TARGET_TRIPLET)/lib/libstdc++.a $(PKG_INSTALL_DIR)/libs
	cp $(PKG_INSTALL_DIR)/$(AUTOCONF_TARGET_TRIPLET)/lib/libsupc++.a $(PKG_INSTALL_DIR)/libs
	# Rename directories from target specific (e.g. "arm-none-eabi") to
	# "_target" and "_gcc_version" to use them in Mybuild.
	$(MV) $(PKG_INSTALL_DIR)/$(AUTOCONF_TARGET_TRIPLET) $(PKG_INSTALL_DIR)/_target
	$(MV) $(PKG_INSTALL_DIR)/_target/include/c++/$(PKG_VER) $(PKG_INSTALL_DIR)/_target/include/c++/_gcc_version
	$(MV) $(PKG_INSTALL_DIR)/_target/include/c++/_gcc_version/$(AUTOCONF_TARGET_TRIPLET) \
		$(PKG_INSTALL_DIR)/_target/include/c++/_gcc_version/_target
	touch $@

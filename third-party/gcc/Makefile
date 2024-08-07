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

PKG_NAME    := gcc
PKG_VER     := $(call option_get_string,gcc_version)
PKG_SOURCES := http://ftp.gnu.org/gnu/gcc/$(PKG_NAME)-$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_PATCHES := gcc_$(PKG_VER).patch

PKG_MD5_6.3.0  := 6e5ea04789678f1250c1b30c4d9ec417
PKG_MD5_8.3.0  := 9972f8c24c02ebcb5a342c1b30de69ff
PKG_MD5_9.3.0  := 9b7e8f6cfad96114e726c752935af58a
PKG_MD5_13.3.0 := f6e5402827861cd5397b3947bc7b8ff2
PKG_MD5        := $(PKG_MD5_$(PKG_VER))

include $(EXTBLD_LIB)

ifeq (arm,$(EMBOX_ARCH))
GCC_TARGET := arm-none-eabi
else ifeq (mips,$(EMBOX_ARCH))
GCC_TARGET := mips-mti-elf
else ifeq (mips64,$(EMBOX_ARCH))
GCC_TARGET := mips-mti-elf
else ifeq (riscv,$(EMBOX_ARCH))
GCC_TARGET := riscv64-unknown-elf
else ifeq (riscv64,$(EMBOX_ARCH))
GCC_TARGET := riscv64-unknown-elf
else ifeq (sparc,$(EMBOX_ARCH))
GCC_TARGET := sparc-elf
else ifeq (x86,$(EMBOX_ARCH))
GCC_TARGET := i386-elf
else
$(error Unsupported architecture: $(EMBOX_ARCH))
endif

GCC_BUILD_DIR   := $(BUILD_DIR)/build
GCC_CPPFLAGS    := -I$(GCC_BUILD_DIR)/gcc/include
LIBGCC_CPPFLAGS := -DLIBGCC2_HAS_XF_MODE=0 -DDO_GLOBAL_CTORS_BODY -DDO_GLOBAL_DTORS_BODY

ifeq (arm,$(EMBOX_ARCH))
LIBSTDCXX_CXXFLAGS := -frtti
else
LIBSTDCXX_CXXFLAGS :=
endif

$(CONFIGURE) :
	$(MKDIR) $(GCC_BUILD_DIR) && cd $(GCC_BUILD_DIR) && ( \
		. $(EMBOX_GCC_ENV) && $(PKG_SOURCE_DIR)/configure \
			--with-cross-host=embox \
			--target=$(GCC_TARGET) \
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
			--enable-threads=posix \
			--enable-hosted-libstdcxx \
			--with-gxx-include-dir=$(PKG_INSTALL_DIR)/include \
			target_configargs="CC=$(EMBOX_GCC) CXX=$(EMBOX_GXX)" \
			CFLAGS_FOR_TARGET="$(GCC_CPPFLAGS) $(LIBGCC_CPPFLAGS)" \
			CXXFLAGS_FOR_TARGET="$(GCC_CPPFLAGS) $(LIBSTDCXX_CXXFLAGS)" \
	)
	touch $@

$(BUILD) :
	cd $(GCC_BUILD_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			GCC_FOR_TARGET=$(EMBOX_GCC) \
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
				AR=true RANLIB=true libgcc-support stmp-int-hdrs CXXDEPMODE=true" \
			TARGET-target-libgcc="CC=$(EMBOX_GCC) PICFLAG= EXTRA_PARTS= all" \
			TARGET-target-libstdc++-v3="CC=$(EMBOX_GCC) CXX=$(EMBOX_GXX) all" \
			all-target-libgcc all-target-libstdc++-v3; \
	)
	touch $@

$(INSTALL) :
	cd $(GCC_BUILD_DIR) && ( \
		$(MAKE) install-target-libgcc; \
		$(MAKE) install-target-libstdc++-v3; \
	)
	$(MV) $(PKG_INSTALL_DIR)/lib/gcc/$(GCC_TARGET)/$(PKG_VER)/libgcc.a $(PKG_INSTALL_DIR)/lib
	$(MV) $(PKG_INSTALL_DIR)/$(GCC_TARGET)/lib/libstdc++.a $(PKG_INSTALL_DIR)/lib
	$(MV) $(PKG_INSTALL_DIR)/$(GCC_TARGET)/lib/libsupc++.a $(PKG_INSTALL_DIR)/lib
	$(MV) $(PKG_INSTALL_DIR)/include/$(GCC_TARGET) $(PKG_INSTALL_DIR)/include/_target
	touch $@

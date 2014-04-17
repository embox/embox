
PKG_NAME := gcc
PKG_VER  := 4.8.1

PKG_SOURCES := http://ftp.gnu.org/gnu/gcc/$(PKG_NAME)-$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

ifeq (arm,$(EMBOX_ARCH))
AUTOCONF_TARGET_TRIPLET := arm-none-eabi
else ifeq (sparc,$(EMBOX_ARCH))
AUTOCONF_TARGET_TRIPLET := sparc-elf
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
	-I$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)-bld/gcc/include

$(CONFIGURE) :
	mkdir -p $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)-bld; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)-bld && ( \
		. $(EMBOX_GCC_ENV) && ../$(PKG_NAME)-$(PKG_VER)/configure \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--disable-multilib \
			--disable-shared \
			--disable-tls \
			--disable-nls \
			--disable-libssp \
			--without-newlib \
			--enable-languages=c,c++ \
			CC_FOR_TARGET=$${EMBOX_CROSS_COMPILE}gcc \
			CXX_FOR_TARGET=$${EMBOX_CROSS_COMPILE}g++ \
			CFLAGS_FOR_TARGET="$(cppflags) $(target_libgcc_cppflags) $${EMBOX_IMPORTED_CFLAGS}" \
			CXXFLAGS_FOR_TARGET="$(cppflags) $${EMBOX_IMPORTED_CXXFLAGS} -fpermissive" \
			LDFLAGS_FOR_TARGET="$${EMBOX_IMPORTED_LDFLAGS}"; \
	)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)-bld && ( \
		. $(EMBOX_GCC_ENV) && make MAKEFLAGS='$${EMBOX_IMPORTED_MAKEFLAGS}' \
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
         	TARGET-gcc="LINKER=true CC=true CXX=true \
				GCC_FOR_TARGET=$${EMBOX_CROSS_COMPILE}gcc \
				AR=true RANLIB=true libgcc-support stmp-int-hdrs" \
         	TARGET-target-libgcc="PICFLAG= EXTRA_PARTS= all" \
         	TARGET-target-libstdc++-v3="SUBDIRS=\"include libsupc++\" \
				all-recursive" \
         	all-target-libgcc all-target-libstdc++-v3; \
	)
	touch $@

$(INSTALL) :
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)-bld/$(AUTOCONF_TARGET_TRIPLET)/libgcc/libgcc.a $(PKG_INSTALL_DIR)/libgcc.a
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)-bld/$(AUTOCONF_TARGET_TRIPLET)/libstdc++-v3/libsupc++/.libs/libsupc++.a $(PKG_INSTALL_DIR)/libsupc++.a
	touch $@

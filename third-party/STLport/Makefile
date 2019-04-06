
PKG_NAME := STLport
PKG_VER  := 5.2.1

PKG_SOURCES := http://download.sourceforge.net/project/stlport/STLport/$(PKG_NAME)-$(PKG_VER).tar.bz2
PKG_MD5     := a8341363e44d9d06a60e03215b38ddde

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

EMBOX_TARGET_CC =$(shell which $(EMBOX_CROSS_COMPILE)gcc)
EMBOX_TARGET_CXX=$(shell which $(EMBOX_CROSS_COMPILE)g++)

ifeq (,$(EMBOX_TARGET_CC))
$(error Cannot find C compiler: $(EMBOX_CROSS_COMPILE)gcc)
endif
ifeq (,$(EMBOX_TARGET_CXX))
$(error Cannot find C++ compiler: $(EMBOX_CROSS_COMPILE)g++)
endif

AT=

STLPORT_DIR   = $(BUILD_DIR)
STLPORT_FLAGS = $(STLPORT_DIR)/flags

EMBOX_IMPORTED_CPPFLAGS += -I../../stlport
EMBOX_IMPORTED_CPPFLAGS += -save-temps=obj

EMBOX_IMPORTED_CPPFLAGS += -DUSE_SPRINTF_INSTEAD
EMBOX_IMPORTED_CPPFLAGS += $(filter -I%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -D%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -U%,$(EMBOX_CPPFLAGS))

EMBOX_IMPORTED_CFLAGS   += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-common,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -march%,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -m%,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-stack-protector,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -g,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -mno-unaligned-access,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += -I$(abspath .)/include

EMBOX_IMPORTED_CXXFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CXXFLAGS += $(subst .,$(ROOT_DIR),$(filter -I%,$(EMBOX_CXXFLAGS)))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-common,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -march%,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -m32,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-stack-protector,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -g,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-exceptions,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-threadsafe-statics,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mno-unaligned-access,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += -I$(abspath .)/include

EMBOX_IMPORTED_MAKEFLAGS += $(filter -j,$(EMBOX_MAKEFLAGS))
ifneq ($(filter -j,$(EMBOX_MAKEFLAGS)),)
EMBOX_IMPORTED_MAKEFLAGS += $(shell nproc)
endif

ifneq (,$(filter -fno-rtti,$(EMBOX_CXXFLAGS)))
EMBOX_RTTI_FLAG          = --without-rtti
else
EMBOX_RTTI_FLAG          =
endif

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure \
			--target=embox \
			--with-cc=$(EMBOX_GCC) \
			--with-cxx=$(EMBOX_GXX) \
			--with-extra-cflags="$(EMBOX_IMPORTED_CFLAGS)" \
			--with-extra-cxxflags="$(EMBOX_IMPORTED_CXXFLAGS)" \
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
	mkdir -p $(PKG_INSTALL_DIR)/../embox-lib/ $(PKG_INSTALL_DIR)/../include/
	cp $(PKG_SOURCE_DIR)/build/lib/obj/embox-gcc/so_g/libstlportg.a $(PKG_INSTALL_DIR)/../embox-lib/
	cp -r -f $(PKG_SOURCE_DIR)/stlport $(PKG_INSTALL_DIR)/../include/
	touch $@

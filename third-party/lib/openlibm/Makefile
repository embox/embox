# https://github.com/JuliaLang/openlibm After merge mips supporting
PKG_NAME := openlibm
PKG_VER := v0.7.0

# PKG_DOWNLOADS_SEPARATE := true
PKG_SOURCES := https://github.com/JuliaMath/openlibm/archive/$(PKG_VER).tar.gz

PKG_MD5 := 3fddbe1683e07a7b2791580d55d5ca7e

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)
include $(ROOT_DIR)/mk/core/compiler.mk

ifeq ($(COMPILER),clang)
COMPILER_SPEC := "USECLANG=1"
else
COMPILER_SPEC := "USEGCC=1"
OLIBM_ARCH ?=
ifneq ($(filter x86 usermode86,$(ARCH)),)
OLIBM_ARCH := i387
endif

ifneq ($(filter mips,$(ARCH)),)
OLIBM_ARCH := mips
endif

ifneq ($(filter ppc powerpc,$(ARCH)),)
OLIBM_ARCH := powerpc
endif

ifeq ($(ARCH), arm)
OLIBM_CPU  := $(subst -mcpu=,,$(filter -mcpu=%,$(CFLAGS)))
ifeq ($(strip $(OLIBM_CPU)),cortex-m7)
# We do not pass -march=armv7e-m because it does not working together with -mcpu=cortex-m7
# See https://bugs.launchpad.net/gcc-arm-embedded/+bug/1592635
OLIBM_ARCH :=""
else
OLIBM_ARCH := $(subst -march=,,$(filter -march=%,$(CFLAGS)))
endif # cortex-m7

ifeq ($(strip $(OLIBM_ARCH)),)
$(error OLIBM_ARCH -march is undefined)
endif # -march is undefined
endif # arm 

endif # COMPILER

$(CONFIGURE) :
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) \
			$(COMPILER_SPEC) \
			ARCH=$(OLIBM_ARCH) \
			OS=Embox \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="-include $(SRC_DIR)/include/sys/cdefs.h -D__ELF__" \
			SFLAGS="-D__ELF__" \
			libopenlibm.a; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/libopenlibm.a $(PKG_INSTALL_DIR)/
	cp $(PKG_SOURCE_DIR)/include/openlibm*.h $(PKG_INSTALL_DIR)/
	touch $@

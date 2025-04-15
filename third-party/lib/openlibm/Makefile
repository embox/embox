
include $(EXTBLD_DEFINES)

PKG_NAME := openlibm
PKG_VER  := 0.8.3

PKG_SOURCES := https://github.com/JuliaMath/openlibm/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := 8bdbe6bf53dfb8790b8ccb98773cafd6
PKG_PATCHES := openlibm.patch

DBL_MANT_DIG  := $(call gcc_macro_get,__DBL_MANT_DIG__)
LDBL_MANT_DIG := $(call gcc_macro_get,__LDBL_MANT_DIG__)

ifneq ($(DBL_MANT_DIG),53)
$(error Invalid DBL_MANT_DIG)
endif

ifeq ($(LDBL_MANT_DIG),53)
LONG_DOUBLE_SIZE := 64
else ifeq ($(LDBL_MANT_DIG),64)
LONG_DOUBLE_SIZE := 80
else ifeq ($(LDBL_MANT_DIG),113)
LONG_DOUBLE_SIZE := 128
else
$(error Invalid LDBL_MANT_DIG)
endif

ifeq ($(COMPILER),clang)
COMPILER_SPEC := "USECLANG=1"
else
COMPILER_SPEC := "USEGCC=1"
endif

OLIBM_CPPFLAGS := -D__ELF__

ifeq ($(EMBOX_ARCH), aarch64)
OLIBM_ARCH := aarch64
else ifeq ($(EMBOX_ARCH), arm)
OLIBM_ARCH := arm
else ifeq ($(EMBOX_ARCH), mips)
OLIBM_ARCH := mips
else ifeq ($(EMBOX_ARCH), mips64)
OLIBM_ARCH := mips
else ifeq ($(EMBOX_ARCH), ppc)
OLIBM_ARCH := powerpc
OLIBM_CPPFLAGS := -D__powerpc__
else ifeq ($(EMBOX_ARCH), riscv)
OLIBM_ARCH := riscv64
else ifeq ($(EMBOX_ARCH), riscv64)
OLIBM_ARCH := riscv64
else ifeq ($(EMBOX_ARCH), usermode86)
OLIBM_ARCH := i387
else ifeq ($(EMBOX_ARCH), x86)
OLIBM_ARCH := i387
else
$(error Unsupported architecture)
endif

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) $(COMPILER_SPEC) \
			OS=Embox \
			CC=$(EMBOX_GCC) \
			ARCH=$(OLIBM_ARCH) \
			CPPFLAGS="$(OLIBM_CPPFLAGS)" \
			LONG_DOUBLE_SIZE=$(LONG_DOUBLE_SIZE) \
			libopenlibm.a; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/libopenlibm.a $(PKG_INSTALL_DIR)/
	cp $(PKG_SOURCE_DIR)/include/openlibm*.h $(PKG_INSTALL_DIR)/
	touch $@

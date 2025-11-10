ifndef __flags_mk
__flags_mk := 1

include mk/core/common.mk

override undefine CFLAGS
override undefine CXXFLAGS
override undefine CPPFLAGS
override undefine ASFLAGS
override undefine ARFLAGS
override undefine LDFLAGS

COMPILER ?= gcc

include $(MKGEN_DIR)/build.mk

ifndef ARCH
$(error ARCH is not defined)
endif

CFLAGS ?=
CXXFLAGS ?=
CPPFLAGS ?=
ASFLAGS ?=
ARFLAGS ?=
LDFLAGS ?=

CROSS_COMPILE ?=

GCC     := $(CROSS_COMPILE)gcc
GXX     := $(CROSS_COMPILE)g++
AR      := $(CROSS_COMPILE)ar
AS      := $(CROSS_COMPILE)as
LD      := $(CROSS_COMPILE)ld
NM      := $(CROSS_COMPILE)nm
OBJDUMP := $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE    := $(CROSS_COMPILE)size

ifeq ($(COMPILER),clang)
CC  := clang
CXX := clang++
else
CC  := $(GCC)
CXX := $(GXX)
endif
CPP := $(CC) -E

EMBOX_GCC := $(TOOLCHAIN_DIR)/embox-gcc
EMBOX_GXX := $(TOOLCHAIN_DIR)/embox-g++

EMBOX_CLANG   := $(TOOLCHAIN_DIR)/embox-clang
EMBOX_CLANGXX := $(TOOLCHAIN_DIR)/embox-clang++

ifeq ($(COMPILER),clang)
EMBOX_CC  := $(EMBOX_CLANG)
EMBOX_CXX := $(EMBOX_CLANGXX)
else
EMBOX_CC  := $(EMBOX_GCC)
EMBOX_CXX := $(EMBOX_GXX)
endif

ifeq ($(COMPILER),gcc)
GCC_VERSION_MAJOR := $(shell echo __GNUC__ | $(GCC) -E -P -)
else
GCC_VERSION_MAJOR :=
endif

ifeq ($(COMPILER),clang)
CLANG_VERSION_MAJOR := $(shell echo __clang_major__ | clang -E -P -)
else
CLANG_VERSION_MAJOR :=
endif

SRC_INCLUDE_PATH := \
	include \
	arch/$(ARCH)/include \
	compat/linux/include \
	compat/posix/include \
	compat/bsd/include \
	compat/libc/include

#
# Preprocessor flags
#
cppflags := \
	-U__linux__ -Ulinux -U__linux \
	-D__unix \
	-D__EMBOX__ \
	-D__EMBOX_VERSION__=\"$(EMBOX_VERSION)\"

ifdef PLATFORM
cppflags += -D__EMBOX_PLATFORM__$(subst -,_,$(PLATFORM))__
endif

cppflags += -I$(INCLUDE_INSTALL_DIR) -I$(SRCGEN_DIR)/include

ifdef GEN_DIST
cppflags += $(addprefix -I$(SRCGEN_DIR)/src/,$(SRC_INCLUDE_PATH))
else
cppflags += $(addprefix -I$(SRC_DIR)/,$(SRC_INCLUDE_PATH))

# Required for "@Generated" headers. See mk/ugly.mk.
cppflags += -I$(SRCGEN_DIR)/src/include
endif

cppflags += -nostdinc -MMD -MP $(CPPFLAGS)
CPPFLAGS := $(cppflags)

EMBOX_EXPORT_CPPFLAGS := $(filter-out -D%" -D%',$(cppflags))

#
# Common compilation flags
#
common_flags := -pipe

# check whether option is supported by sending all errors to /dev/null
# if not supported 'echo true' will be ANDed with zero output
# else it is supported
# this flag makes the pathnames to sources for debug symbols relative
# to the the root embox directory, keep that in mind in order for the debugger
# to properly find sources at runtime.
file_prefix_map_supported := $(shell $(CPP) /dev/zero -ffile-prefix-map=./=. 2>/dev/null && echo true)
ifeq ($(file_prefix_map_supported),true)
common_flags += -ffile-prefix-map=`pwd`=.
endif

#
# Assembler flags
#
asflags := $(common_flags) $(CFLAGS)
ASFLAGS := $(asflags)

#
# Common C/C++ compiler flags
#
common_ccflags := $(common_flags)
common_ccflags += -fno-strict-aliasing
common_ccflags += -fno-common
common_ccflags += -fno-stack-protector
common_ccflags += -fno-pic
common_ccflags += -Wall -Werror -Wformat -Wundef
common_ccflags += -Wno-trigraphs -Wno-char-subscripts

ifeq ($(COMPILER),gcc)
# This option conflicts with some third-party stuff, so we disable it.
CFLAGS += -Wno-misleading-indentation
# GCC 6 seems to have many library functions declared as __nonnull__, like
# fread, fwrite, fprintf, ...  Since accessing NULL in embox without MMU
# support could cause real damage to whole system in contrast with segfault of
# application, we decided to keep explicit null checks and disable the warning.
CFLAGS += -Wno-nonnull-compare
ifeq ($(GCC_VERSION_MAJOR),7)
common_ccflags += -Wno-error=format-truncation=
common_ccflags += -Wno-error=alloc-size-larger-than=
endif
ifeq ($(ARCH),x86)
ifeq ($(shell expr $(GCC_VERSION_MAJOR) \>= 8), 1)
# This fixes gdb corrupt stack when debugging with QEMU.
# This options is added here to suppress addbr32 instr generation,
# which is incorrectly interpreted by QEMU.
common_ccflags += -fcf-protection=none
endif
# There are some troubles on x86 with optimizations (O1 and more)
# without this flag*/
common_ccflags += -fno-omit-frame-pointer
endif
endif

ifeq ($(COMPILER),clang)
common_ccflags += -Wno-gnu
common_ccflags += -fshort-enums
common_ccflags += -Wno-address-of-packed-member
common_ccflags += -meabi gnu
ifeq ($(shell expr $(CLANG_VERSION_MAJOR) \>= 16), 1)
common_ccflags += -Wno-missing-multilib
endif
endif

#
# C compiler flags
#
cflags := -std=gnu99 $(common_ccflags) $(CFLAGS)
CFLAGS := $(cflags)

#
# C++ compiler flags
#
cxxflags := $(common_ccflags) $(CXXFLAGS)
CXXFLAGS := $(cxxflags)

#
# Linker flags
#
ldflags := -static -nostdlib -znoexecstack $(LDFLAGS)
LDFLAGS := $(ldflags)

#
# Archiver flags
#
ARFLAGS = rcs

#
# Export variables for use in scripts
#
export EMBOX_CROSS_COMPILE := $(CROSS_COMPILE)

export EMBOX_CC
export EMBOX_CXX

export EMBOX_GCC
export EMBOX_GXX

export EMBOX_CLANG
export EMBOX_CLANGXX

endif # __flags_mk

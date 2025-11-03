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

BUILD_DEPS_LDFLAGS ?=
BUILD_DEPS_CPPFLAGS_BEFORE ?=
BUILD_DEPS_CPPFLAGS_AFTER ?=
MODULE_CPPFLAGS ?=

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

#
# We use '+', because we want to call external build as recursive sub-make.
# From https://www.gnu.org/software/make/manual/html_node/MAKE-Variable.html:
#
# "This special feature is only enabled if the MAKE variable appears directly
# in the recipe: it does not apply if the MAKE variable is referenced through
# expansion of another variable. In the latter case you must use the '+' token
# to get these special effects."
#
# Since $(MAKE) here is referenced through $(EXTERNAL_MAKE) it's fine to add
# additional '+'.
#
EXTERNAL_MAKE = \
	+$(MAKE) -C $(dir $(my_file)) -f $(ROOT_DIR)/mk/extbld/build.mk $(EXTERNAL_MAKE_FLAGS)

EXTERNAL_MAKE_QT = \
	$(MKDIR) $(mod_build_dir) && \
	$(CP) $(EXTERNAL_BUILD_DIR)/third_party/qt/core/install/.qmake.cache $(mod_build_dir) && \
	$(EXTERNAL_BUILD_DIR)/third_party/qt/core/install/bin/qmake \
		INCLUDEPATH+='$(subst -I,,$(BUILD_DEPS_CPPFLAGS_BEFORE) $(BUILD_DEPS_CPPFLAGS_AFTER))' \
		LIBS+='$(BUILD_DEPS_LDFLAGS)' \
		$${TARGET:-$(dir $(my_file))} \
		-o $(abspath $(mod_build_dir))/Makefile && \
	$(MAKE) -C $(mod_build_dir) $(EXTERNAL_MAKE_FLAGS)

EXTERNAL_MAKE_FLAGS = \
	MAKEFLAGS= \
	MOD_DIR=$(abspath $(dir $(my_file))) \
	MOD_BUILD_DIR=$(abspath $(mod_build_dir)) \
	AUTOCONF_ARCH=$(AUTOCONF_ARCH) \
	AUTOCONF_TARGET_TRIPLET=$(AUTOCONF_ARCH)-unknown-none \
	COMPILER=$(COMPILER) \
	EMBOX_ARCH=$(ARCH) \
	EMBOX_GCC=$(EMBOX_GCC) \
	EMBOX_GXX=$(EMBOX_GXX) \
	EMBOX_CROSS_COMPILE=$(CROSS_COMPILE) \
	EMBOX_MAKEFLAGS='$(MAKEFLAGS)' \
	EMBOX_IMPORTED_MAKEFLAGS='$(filter -j --jobserver-auth=% --jobserver-fds=%, $(MAKEFLAGS))' \
	EMBOX_CFLAGS='$(CFLAGS)' \
	EMBOX_CXXFLAGS='$(CXXFLAGS)' \
	EMBOX_CPPFLAGS='$(BUILD_DEPS_CPPFLAGS_BEFORE) $(EMBOX_EXPORT_CPPFLAGS) $(BUILD_DEPS_CPPFLAGS_AFTER)' \
	EMBOX_LDFLAGS='$(LDFLAGS) $(BUILD_DEPS_LDFLAGS)' \
	EMBOX_DEPS_CPPFLAGS_BEFORE='$(BUILD_DEPS_CPPFLAGS_BEFORE)' \
	EMBOX_DEPS_CPPFLAGS_AFTER='$(BUILD_DEPS_CPPFLAGS_AFTER)' \
	EMBOX_MODULE_CPPFLAGS='$(MODULE_CPPFLAGS)'

MAIN_STRIPPING = \
	$(MAKE) -f $(ROOT_DIR)/mk/main-stripping.mk \
	$(EXTERNAL_MAKE_FLAGS) TARGET_APP='$(module_id)' FILE_APP='$(abspath $@)'

LOADABLE_MAKE = \
	$(MAKE) \
	-C $(dir $(my_file)) \
	-f $(ROOT_DIR)/mk/script/loadable-build.mk \
	$(EXTERNAL_MAKE_FLAGS)
	
mod_build_dir = $(EXTERNAL_BUILD_DIR)/$(mod_path)

#
# Expand user defined flags and append them after default ones.
#

cppflags_macros := \
	-U__linux__ -Ulinux -U__linux \
	-D__unix \
	-D__EMBOX__ \
	-D__EMBOX_VERSION__=\"$(EMBOX_VERSION)\"

ifdef PLATFORM
cppflags_macros += -D__EMBOX_PLATFORM__$(subst -,_,$(PLATFORM))__
endif

SRC_INCLUDE_PATH := \
	include \
	arch/$(ARCH)/include \
	compat/linux/include \
	compat/posix/include \
	compat/bsd/include \
	compat/libc/include

ifdef GEN_DIST
cppflags_src_include := $(addprefix -I$(SRCGEN_DIR)/src/,$(SRC_INCLUDE_PATH))
else
cppflags_src_include := $(addprefix -I$(SRC_DIR)/,$(SRC_INCLUDE_PATH))
endif

# Preprocessor flags
cppflags := \
	$(cppflags_macros) \
	-I$(INCLUDE_INSTALL_DIR) \
	-I$(SRCGEN_DIR)/include \
	-I$(SRCGEN_DIR)/src/include \
	$(cppflags_src_include) \
	-nostdinc \
	-MMD -MP \
	$(CPPFLAGS)

CPPFLAGS = $(cppflags)
EMBOX_EXPORT_CPPFLAGS = $(filter-out -D%" -D%',$(cppflags))

common_flags := -pipe

# Assembler flags
asflags := $(common_flags) $(CFLAGS)
ASFLAGS = $(asflags)

common_ccflags := $(common_flags)
common_ccflags += -fno-strict-aliasing
common_ccflags += -fno-common
common_ccflags += -fno-stack-protector
common_ccflags += -fno-pic
common_ccflags += -Wall -Werror -Wformat -Wundef
common_ccflags += -Wno-trigraphs -Wno-char-subscripts

# check whether option is supported by sending all errors to /dev/null
# if not supported 'echo true' will be ANDed with zero output
# else it is supported
# this flag makes the pathnames to sources for debug symbols relative
# to the the root embox directory, keep that in mind in order for the debugger
# to properly find sources at runtime.
file_prefix_map_supported := $(shell $(CPP) /dev/zero -ffile-prefix-map=./=. 2>/dev/null && echo true)
ifneq ($(debug_prefix_map_supported),)
common_ccflags += -ffile-prefix-map=`$(ROOT_DIR)`=.
endif

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

# C compiler flags
cflags := -std=gnu99 $(common_ccflags) $(CFLAGS)
CFLAGS = $(cflags)

# C++ compiler flags
cxxflags := $(common_ccflags) $(CXXFLAGS)
CXXFLAGS = $(cxxflags)

# Linker flags
ldflags := -static -nostdlib $(LDFLAGS)
LDFLAGS = $(ldflags)

# Archiver flags
ARFLAGS = rcs

ifeq ($(ARCH),x86)
AUTOCONF_ARCH := i386
else
AUTOCONF_ARCH := $(ARCH)
endif

LIBGCC_FINDER = $(GCC) $(CFLAGS)

EMBOX_GCC := $(TOOLCHAIN_DIR)/embox-gcc
EMBOX_GXX := $(TOOLCHAIN_DIR)/embox-g++

EMBOX_CLANG   := $(TOOLCHAIN_DIR)/embox-clang
EMBOX_CLANGXX := $(TOOLCHAIN_DIR)/embox-clang++

endif # __flags_mk

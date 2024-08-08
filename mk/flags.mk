
include mk/core/common.mk

CFLAGS ?=
CXXFLAGS ?=
CPPFLAGS ?=
ASFLAGS ?=
ARFLAGS ?=
LDFLAGS ?=
COMPILER ?=
BUILD_DEPS_CPPFLAGS_BEFORE ?=
BUILD_DEPS_CPPFLAGS_AFTER ?=
BUILD_DEPS_LDFLAGS ?=

CROSS_COMPILE ?=
CXX     ?= $(CROSS_COMPILE)g++
AR      ?= $(CROSS_COMPILE)ar
AS      ?= $(CROSS_COMPILE)as
LD      ?= $(CROSS_COMPILE)ld
NM      ?= $(CROSS_COMPILE)nm
OBJDUMP ?= $(CROSS_COMPILE)objdump
OBJCOPY ?= $(CROSS_COMPILE)objcopy
SIZE    ?= $(CROSS_COMPILE)size

ifeq ($(COMPILER),clang)
CC      := clang
# for clang LIBGCC_FINDER will be set externally to arm-none-eabi-gcc or something like that
else
CC      := $(CROSS_COMPILE)gcc
LIBGCC_FINDER=$(CC) $(CFLAGS)
endif
CPP     ?= $(CC) -E

CPPFLAGS += -D__EMBOX_VERSION__=\"$(EMBOX_VERSION)\"

comma_sep_list = $(subst $(\s),$(,),$(strip $1))

COVERAGE_CFLAGS ?= -finstrument-functions \
   -finstrument-functions-exclude-function-list=$(call comma_sep_list, \
			symbol_lookup \
			__cyg_profile_func_enter \
			__cyg_profile_func_exit \
			__coverage_func_enter \
			__coverage_func_exit \
			bitmap_set_bit)

PROFILING_CFLAGS ?= -finstrument-functions \
   -finstrument-functions-exclude-function-list=$(call comma_sep_list, \
			__cyg_profile_func_enter \
			__cyg_profile_func_exit \
			cyg_tracing_profiler_enter \
			cyg_tracing_profiler_exit \
			__coverage_func_enter \
			__coverage_func_exit \
			trace_block_func_enter \
			trace_block_func_exit \
			get_trace_block_hash \
			cmp_trace_blocks \
			trace_block_enter \
			trace_block_leave \
			__tracepoint_handle \
			get_profiling_mode \
			set_profiling_mode)

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
	+$(MAKE) -C $(dir $(my_file)) $(EXTERNAL_MAKE_FLAGS)

EXTERNAL_MAKE_PRO = \
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
	$(foreach path_var, \
			ROOT_DIR \
			EMBOX_ROOT_DIR \
			CONF_DIR \
			TEMPLATES_DIR \
			SRC_DIR \
			THIRDPARTY_DIR \
			PLATFORM_DIR \
			PROJECT_DIR \
			SUBPLATFORM_TEMPLATE_DIR \
			EXTERNAL_BUILD_DIR \
			LOADABLE_DIR \
			DOC_DIR \
			BIN_DIR \
			OBJ_DIR \
			GEN_DIR \
			SRCGEN_DIR \
			MKGEN_DIR \
			AUTOCONF_DIR \
			ROOTFS_DIR \
			INCLUDE_INSTALL_DIR \
			USER_ROOTFS_DIR \
			DOT_DIR \
			DOCS_OUT_DIR \
			CACHE_DIR, \
		$(path_var)=$(abspath $($(path_var)))) \
	BUILD_DIR=$(abspath $(mod_build_dir)) \
	COMPILER=$(COMPILER) \
	EMBOX_ARCH='$(ARCH)' \
	EMBOX_CROSS_COMPILE='$(CROSS_COMPILE)' \
	EMBOX_MAKEFLAGS='$(MAKEFLAGS)' \
	EMBOX_IMPORTED_MAKEFLAGS='$(filter -j --jobserver-auth=% --jobserver-fds=%, $(MAKEFLAGS))' \
	EMBOX_CFLAGS='$(CFLAGS)' \
	EMBOX_CXXFLAGS='$(CXXFLAGS)' \
	EMBOX_DEPS_CPPFLAGS_BEFORE='$(BUILD_DEPS_CPPFLAGS_BEFORE)' \
	EMBOX_DEPS_CPPFLAGS_AFTER='$(BUILD_DEPS_CPPFLAGS_AFTER)' \
	EMBOX_CPPFLAGS='$(EMBOX_EXPORT_CPPFLAGS)' \
	EMBOX_LDFLAGS='$(LDFLAGS) $(BUILD_DEPS_LDFLAGS)'

mod_build_dir = $(EXTERNAL_BUILD_DIR)/$(mod_path)

EXTERNAL_OBJ_DIR =^BUILD/extbld/^MOD_PATH#

LOADABLE_MAKE = \
	$(MAKE) \
	-C $(dir $(my_file)) \
	-f $(EMBOX_ROOT_DIR)/mk/script/loadable-build.mk \
	$(EXTERNAL_MAKE_FLAGS)

ifneq ($(patsubst N,0,$(patsubst n,0,$(or $(value NDEBUG),0))),0)
override CPPFLAGS += -DNDEBUG
override NDEBUG := 1
else
override NDEBUG :=
endif

ifdef OPTIMIZE

override OPTIMIZE := $(strip $(OPTIMIZE:-O%=%))
__optimize_valid_values := s 0 1 2 3 4 5 99
__optimize_invalid := $(filter-out $(__optimize_valid_values),$(OPTIMIZE))
ifneq ($(__optimize_invalid),)
$(error Invalid value for OPTIMIZE flag: $(__optimize_invalid). \
  Valid values are: $(__optimize_valid_values))
endif

ifneq ($(words $(OPTIMIZE)),1)
$(error Only single value for OPTIMIZE flag is permitted)
endif

override CFLAGS += -O$(OPTIMIZE)
override CXXFLAGS += -O$(OPTIMIZE)

endif

# Expand user defined flags and append them after default ones.

__srcgen_includes_fn = \
	$(addprefix $(call $1,$(SRCGEN_DIR))/, \
		src/include \
		src/arch/$(ARCH)/include)
__srcgen_includes := $(call __srcgen_includes_fn,id)
$(and $(shell $(MKDIR) $(__srcgen_includes)),)

cppflags_fn = \
	-U__linux__ -Ulinux -U__linux \
	-D__EMBOX__ \
	-D__unix \
	-imacros $(call $1,$(AUTOCONF_DIR))/config.lds.h \
	-I$(call $1,$(INCLUDE_INSTALL_DIR)) \
	-I$(call $1,$(SRC_DIR))/include \
	-I$(call $1,$(SRC_DIR))/arch/$(ARCH)/include \
	-I$(call $1,$(SRCGEN_DIR))/include \
	-I$(call $1,$(SRCGEN_DIR))/src/include \
	$(addprefix -I,$(call __srcgen_includes_fn,$1)) \
	$(if $(value PLATFORM),-I$(call $1,$(PLATFORM_DIR))/$(PLATFORM)/include) \
	-I$(call $1,$(SRC_DIR))/compat/linux/include \
	-I$(call $1,$(SRC_DIR))/compat/posix/include \
	-I$(call $1,$(SRC_DIR))/compat/bsd/include \
	-I$(call $1,$(SRC_DIR))/compat/libc/include \
	-nostdinc \
	-MMD -MP# -MT $@ -MF $(@:.o=.d)

# Preprocessor flags
cppflags := $(CPPFLAGS)
override CPPFLAGS  = $(call cppflags_fn,id) $(cppflags)
EMBOX_EXPORT_CPPFLAGS = $(call cppflags_fn,abspath)
EMBOX_EXPORT_CPPFLAGS += $(filter-out -D%" -D%',$(cppflags))

override COMMON_FLAGS := -pipe

PREFIX_MAP:=TRUE
ifdef PREFIX_MAP
# check whether option is supported by sending all errors to /dev/null
# if not supported 'echo true' will be ANDed with zero output
# else it is supported
# backslashed pwd at execution time in shell will produce root embox directory
# this flag makes the pathnames to sources for debug symbols relative
# to the the root embox directory, keep that in mind in order for the debugger
# to properly find sources at runtime. If you don't want relative pathnames
# for sources, comment the line PREFIX_MAP:=TRUE above
debug_prefix_map_supported:=$(shell $(CPP) /dev/zero -fdebug-prefix-map=./=. 2>/dev/null && echo true)
ifneq ($(debug_prefix_map_supported),)
override COMMON_FLAGS += -fdebug-prefix-map=`pwd`=.
endif
endif

# Assembler flags
asflags := $(CFLAGS)
override ASFLAGS = $(COMMON_FLAGS)
override ASFLAGS += $(asflags)

override COMMON_CCFLAGS := $(COMMON_FLAGS)
override COMMON_CCFLAGS += -fno-strict-aliasing -fno-common
override COMMON_CCFLAGS += -fno-stack-protector
override COMMON_CCFLAGS += -fno-pic
override COMMON_CCFLAGS += -Wall -Werror
override COMMON_CCFLAGS += -Wundef -Wno-trigraphs -Wno-char-subscripts

ifeq ($(COMPILER),clang)
	override COMMON_CCFLAGS += -Wno-gnu-designator
else
	override GCC_VERSION := $(word 3,$(shell $(CC) --version 2>&1 | head -n1))
	override GCC_VERSION_MAJOR := $(word 1,$(subst ., ,$(GCC_VERSION)))

ifeq ($(GCC_VERSION_MAJOR),7)
	override COMMON_CCFLAGS += -Wno-error=format-truncation=
	override COMMON_CCFLAGS += -Wno-error=alloc-size-larger-than=
endif
ifeq ($(ARCH),x86)
	ifeq ($(shell expr $(GCC_VERSION_MAJOR) \>= 8), 1)
		# This fixes gdb corrupt stack when debugging with QEMU.
		# This options is added here to suppress addbr32 instr generation,
		# which is incorrectly interpreted by QEMU.
		override COMMON_CCFLAGS += -fcf-protection=none
	endif

	# There are some troubles on x86 with optimizations (O1 and more)
	# without this flag*/
	override COMMON_CCFLAGS += -fno-omit-frame-pointer
endif
endif

override COMMON_CCFLAGS += -Wformat

cxxflags := $(CXXFLAGS)
override CXXFLAGS = $(COMMON_CCFLAGS)
#override CXXFLAGS += -fno-rtti
#override CXXFLAGS += -fno-exceptions
#override CXXFLAGS += -fno-threadsafe-statics
#	C++ has build-in type bool
override CXXFLAGS += -DSTDBOOL_H_
override CXXFLAGS += $(cxxflags)

# Compiler flags
cflags := $(CFLAGS)
override CFLAGS  = $(COMMON_CCFLAGS)
override CFLAGS += -std=gnu99
#override CFLAGS += -fexceptions
override CFLAGS += $(cflags)

ifneq ($(COMPILER),clang)
ifneq ($(COMPILER),lcc)
	# Not clang nor lcc means gcc
	# This option conflicts with some third-party stuff, so we disable it.
	override CFLAGS += -Wno-misleading-indentation

	# GCC 6 seems to have many library functions declared as __nonnull__, like
	# fread, fwrite, fprintf, ...  Since accessing NULL in embox without MMU
	# support could cause real damage to whole system in contrast with segfault of
	# application, we decided to keep explicit null checks and disable the warning.
	override CFLAGS += -Wno-nonnull-compare
endif
endif


# Linker flags
ldflags := $(LDFLAGS)
override LDFLAGS  = -static -nostdlib
override LDFLAGS += $(ldflags)

override ARFLAGS = rcs


CCFLAGS ?=

INCLUDES_FROM_FLAGS := \
	$(patsubst -I%,%,$(filter -I%,$(CPPFLAGS) $(CXXFLAGS)))

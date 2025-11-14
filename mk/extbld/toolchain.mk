
include mk/flags.mk
include $(SRCGEN_DIR)/image.rule.mk

# Ignore warning options and -MMD -MP
EMBOX_IMPORTED_CPPFLAGS := $(filter-out -MMD -MP -W%,$(EMBOX_EXPORT_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -Wa$(,)% -Wp$(,)% -Wl$(,)%,$(EMBOX_EXPORT_CPPFLAGS))

# Ignore warning options and -std=<standard>
EMBOX_IMPORTED_CFLAGS   := $(filter-out -std=% -W%,$(CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -Wa$(,)% -Wp$(,)% -Wl$(,)%,$(CFLAGS))

# Only machine-dependent options
EMBOX_IMPORTED_M_CFLAGS := $(filter -m% -EL -EB,$(CFLAGS))
ifeq ($(COMPILER),clang)
EMBOX_IMPORTED_M_CFLAGS += $(filter --target=%,$(subst -target ,--target=,$(strip $(CFLAGS))))
endif

# Ignore warning options and -std=<standard>
EMBOX_IMPORTED_CXXFLAGS := $(filter-out -std=% -W%,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -Wa$(,)% -Wp$(,)% -Wl$(,)%,$(CXXFLAGS))

# Only -static -nostdlib -z<arg> and machine-dependent options
EMBOX_IMPORTED_LDFLAGS  := $(filter -static -nostdlib -EL -EB,$(LDFLAGS))
EMBOX_IMPORTED_LDFLAGS  += $(addprefix -Wl$(,),$(filter -z%,$(subst -z ,-z,$(strip $(LDFLAGS)))))
EMBOX_IMPORTED_LDFLAGS  += $(addprefix -Wl$(,),$(filter -m%,$(subst -m ,-m,$(strip $(LDFLAGS)))))

# In GCC 14 some warnings are reported as errors. Downgrade these errors to warnings.
ifeq ($(COMPILER),gcc)
ifeq ($(shell expr $(GCC_VERSION_MAJOR) \>= 14), 1)
EMBOX_IMPORTED_CFLAGS   += -fpermissive
EMBOX_IMPORTED_CXXFLAGS += -fpermissive
endif
endif

ifdef DIST_GEN
EMBOX_IMPORTED_CPPFLAGS := $(filter-out -I%,$(subst -I ,-I,$(strip $(EMBOX_IMPORTED_CPPFLAGS))))
EMBOX_IMPORTED_CPPFLAGS += -I\$${EMBOX_DIST_INC_DIR}
endif # DIST_GEN

ifeq ($(ARCH),microblaze)
# microblaze compiler wants vendor's xillinx.ld if no lds provided from command line.
# Make it happy with empty lds
_empty_lds_hack:=$(abspath $(SRCGEN_DIR))/empty.lds
$(shell touch $(_empty_lds_hack))
EMBOX_IMPORTED_LDFLAGS += -T $(_empty_lds_hack)
endif

# Enable garbage collection of unused input sections
EMBOX_IMPORTED_LDFLAGS_FULL := -Wl,--gc-sections

LD_DISABLE_RELAXATION ?= n
ifeq ($(LD_DISABLE_RELAXATION),y)
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--no-relax
else
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--relax
endif

ifdef DIST_GEN
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,-T,\$${EMBOX_DIST_LIB_DIR}/image.lds
EMBOX_IMPORTED_LDFLAGS_FULL += \$${EMBOX_DIST_LIB_DIR}/embox.o
EMBOX_IMPORTED_LDFLAGS_FULL += \$${EMBOX_DIST_LIB_DIR}/embox.a
else
# Use `main` as the explicit symbol for beginning execution of your program.
# If there is no --entry=main flag, the test function section
# will be discarded during garbage collection. This flag is needed to check
# the implementation of functions by `configure` scripts
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--entry=main

EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,-T,$(OBJ_DIR)/image.lds
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=__symbol_table=0,--defsym=__symbol_table_size=0
EMBOX_IMPORTED_LDFLAGS_FULL += $(OBJ_DIR)/embox-1.o
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--start-group
EMBOX_IMPORTED_LDFLAGS_FULL += $(__image_ld_libs1)
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--end-group
endif # DIST_GEN

# Escape backticks to prevent command substitution (for example `pwd`).
escape_backticks = $(strip $(subst `,\`,$1))
EMBOX_IMPORTED_CFLAGS := $(call escape_backticks,$(EMBOX_IMPORTED_CFLAGS))
EMBOX_IMPORTED_CXXFLAGS := $(call escape_backticks,$(EMBOX_IMPORTED_CXXFLAGS))

embox_gcc   := $(EMBOX_GCC) $(EMBOX_GXX)
embox_clang := $(EMBOX_CLANG) $(EMBOX_CLANGXX)

embox_binutils := \
	$(TOOLCHAIN_DIR)/embox-ar \
	$(TOOLCHAIN_DIR)/embox-ranlib \
	$(TOOLCHAIN_DIR)/embox-nm \
	$(TOOLCHAIN_DIR)/embox-size

ifdef DIST_GEN
embox_gcc := $(subst $(TOOLCHAIN_DIR),$(DIST_BIN_DIR),$(embox_gcc))
embox_clang := $(subst $(TOOLCHAIN_DIR),$(DIST_BIN_DIR),$(embox_clang))
embox_binutils := $(subst $(TOOLCHAIN_DIR),$(DIST_BIN_DIR),$(embox_binutils))
endif # DIST_GEN

embox_toolchain := $(embox_binutils)

ifeq ($(COMPILER),gcc)
embox_toolchain += $(embox_gcc)
endif

ifeq ($(COMPILER),clang)
embox_toolchain += $(embox_clang)
endif

.PHONY : all
all : $(embox_toolchain)

$(embox_toolchain) : $(MKGEN_DIR)/build.mk $(MKGEN_DIR)/image.rule.mk
$(embox_toolchain) : | $(TOOLCHAIN_DIR)

$(TOOLCHAIN_DIR) :
	@$(MKDIR) $@

$(embox_gcc) $(embox_clang) :
	@COMPILER="$(COMPILER)" \
		EMBOX_IMPORTED_CPPFLAGS="$(EMBOX_IMPORTED_CPPFLAGS)" \
		EMBOX_IMPORTED_CFLAGS="$(EMBOX_IMPORTED_CFLAGS)" \
		EMBOX_IMPORTED_M_CFLAGS="$(EMBOX_IMPORTED_M_CFLAGS)" \
		EMBOX_IMPORTED_CXXFLAGS="$(EMBOX_IMPORTED_CXXFLAGS)" \
		EMBOX_IMPORTED_LDFLAGS="$(EMBOX_IMPORTED_LDFLAGS)" \
		EMBOX_IMPORTED_LDFLAGS_FULL="$(EMBOX_IMPORTED_LDFLAGS_FULL)" \
		$(ROOT_DIR)/mk/extbld/gen_compilers.sh > $@
	@chmod +x $@

$(embox_binutils) :
	@$(ROOT_DIR)/mk/extbld/gen_binutils.sh > $@
	@chmod +x $@


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

# Ignore warning options and -std=<standard>
EMBOX_IMPORTED_CXXFLAGS := $(filter-out -std=% -W%,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -Wa$(,)% -Wp$(,)% -Wl$(,)%,$(CXXFLAGS))

# Only -static -nostdlib and machine-dependent options
EMBOX_IMPORTED_LDFLAGS  := $(filter -static -nostdlib -EL -EB,$(LDFLAGS))
EMBOX_IMPORTED_LDFLAGS  += $(addprefix -Wl$(,),$(filter -m%,$(subst -m ,-m,$(LDFLAGS))))

# In GCC 14 some warnings are reported as errors. Downgrade these errors to warnings.
_gnuc_major := $(shell echo __GNUC__ | $(CPP) -P -)
ifeq ($(_gnuc_major),$(filter $(_gnuc_major),15 14))
EMBOX_IMPORTED_CPPFLAGS += -fpermissive
endif

ifeq ($(ARCH),microblaze)
# microblaze compiler wants vendor's xillinx.ld if no lds provided from command line.
# Make it happy with empty lds
_empty_lds_hack:=$(abspath $(SRCGEN_DIR))/empty.lds
$(shell touch $(_empty_lds_hack))
EMBOX_IMPORTED_LDFLAGS += -T $(_empty_lds_hack)
endif

EMBOX_IMPORTED_LDFLAGS_FULL :=
LD_DISABLE_RELAXATION ?= n
ifeq (n,$(LD_DISABLE_RELAXATION))
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--relax
else
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--no-relax
endif
# Enable garbage collection of unused input sections
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--gc-sections
# Use `main` as the explicit symbol for beginning execution of your program.
# If there is no --entry=main flag, the test function section
# will be discarded during garbage collection. This flag is needed to check
# the implementation of functions by `configure` scripts
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--entry=main
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,-T,$(abspath $(OBJ_DIR))/mk/image.lds
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=__symbol_table=0,--defsym=__symbol_table_size=0
EMBOX_IMPORTED_LDFLAGS_FULL += $(abspath $(OBJ_DIR))/embox.o
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--start-group
EMBOX_IMPORTED_LDFLAGS_FULL += $(__image_ld_libs1:.%=-Wl,$(abspath $(ROOT_DIR))%)
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--end-group

# We are not including user ld scripts in flags, since it is not required to do
# working object, only compilation errors are matter. If required, place it below
# --relax flag and perfrom 2 phase link at arch-unknown-embox, first with user lds,
# next with image.lds
#EMBOX_IMPORTED_LDFLAGS_FULL += $(__image_ld_scripts1:.%=-Wl,-T,$(abspath $(ROOT_DIR))%)

ifdef GEN_DIST
root2dist = $(strip $(subst $(DIST_BASE_DIR),$${EMBOX_DIST_BASE_DIR},$1))
else
root2dist = $1
endif

embox_compilers := $(EMBOX_GCC) $(EMBOX_GXX)
embox_binutils  := \
	$(TOOLCHAIN_DIR)/embox-ar \
	$(TOOLCHAIN_DIR)/embox-ranlib \
	$(TOOLCHAIN_DIR)/embox-nm \
	$(TOOLCHAIN_DIR)/embox-size

embox_toolchain := $(embox_compilers) $(embox_binutils)

.PHONY : all
all : $(embox_toolchain)

$(embox_toolchain) : $(MKGEN_DIR)/build.mk $(MKGEN_DIR)/image.rule.mk
$(embox_toolchain) : | $(TOOLCHAIN_DIR)

$(TOOLCHAIN_DIR) :
	@$(MKDIR) $@

$(embox_compilers) :
	@cat $(ROOT_DIR)/mk/extbld/compiler_start.sh                                            > $@
	@echo EMBOX_IMPORTED_CPPFLAGS='"$(call root2dist,$(EMBOX_IMPORTED_CPPFLAGS))"'         >> $@
	@echo EMBOX_IMPORTED_CFLAGS='"$(call root2dist,$(EMBOX_IMPORTED_CFLAGS))"'             >> $@
	@echo EMBOX_IMPORTED_M_CFLAGS='"$(call root2dist,$(EMBOX_IMPORTED_M_CFLAGS))"'         >> $@
	@echo EMBOX_IMPORTED_CXXFLAGS='"$(call root2dist,$(EMBOX_IMPORTED_CXXFLAGS))"'         >> $@
	@echo EMBOX_IMPORTED_LDFLAGS='"$(call root2dist,$(EMBOX_IMPORTED_LDFLAGS))"'           >> $@
	@echo EMBOX_IMPORTED_LDFLAGS_FULL='"$(call root2dist,$(EMBOX_IMPORTED_LDFLAGS_FULL))"' >> $@
	@cat $(ROOT_DIR)/mk/extbld/compiler_end.sh                                             >> $@
	@chmod +x $@

$(embox_binutils) :
	@cat $(ROOT_DIR)/mk/extbld/utils_stub.sh > $@
	@chmod +x $@

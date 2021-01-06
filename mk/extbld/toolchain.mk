
include mk/core/common.mk

include $(MKGEN_DIR)/build.mk
include mk/flags.mk
include $(SRCGEN_DIR)/image.rule.mk

EMBOX_IMPORTED_CPPFLAGS  = $(filter -D% -U% -I% -nostdinc,$(filter-out -D"% -D'%,$(EMBOX_EXPORT_CPPFLAGS)))

EMBOX_IMPORTED_CFLAGS    = $(filter -g% -f% -m% -O% -G% -E% -Wa%,$(CFLAGS))
EMBOX_IMPORTED_CXXFLAGS  = $(filter -g% -f% -m% -O% -G% -E% -Wa% -std=%,$(CXXFLAGS))

EMBOX_IMPORTED_LDFLAGS   = $(filter -static -nostdlib -E%,$(LDFLAGS))
EMBOX_IMPORTED_LDFLAGS  += $(addprefix -Wl$(,),$(filter -m elf_i386,$(LDFLAGS)))
ifeq ($(ARCH),microblaze)
# microblaze compiler wants vendor's xillinx.ld if no lds provided from command line.
# Make it happy with empty lds
_empty_lds_hack:=$(abspath $(SRCGEN_DIR))/empty.lds
$(shell touch $(_empty_lds_hack))
EMBOX_IMPORTED_LDFLAGS += -T $(_empty_lds_hack)
endif

EMBOX_IMPORTED_LDFLAGS_FULL  =
LD_DISABLE_RELAXATION ?= n
ifeq (n,$(LD_DISABLE_RELAXATION))
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--relax
else
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--no-relax
endif
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--gc-sections
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
root2dist = $(strip \
	$(subst $(abspath $(DIST_BASE_DIR)),$${EMBOX_DIST_BASE_DIR}, \
		$(subst $(abspath $(ROOT_DIR)),$${EMBOX_ROOT_DIR},$1)))
else
root2dist = $1
endif

$(EMBOX_GCC_ENV): $(MKGEN_DIR)/build.mk $(MKGEN_DIR)/image.rule.mk
$(EMBOX_GCC_ENV): mk/flags.mk mk/extbld/toolchain.mk
$(EMBOX_GCC_ENV): | $(dir $(EMBOX_GCC_ENV))
	@echo EMBOX_CROSS_COMPILE='"$(CROSS_COMPILE)"'                       > $@
	@echo EMBOX_IMPORTED_CPPFLAGS='"$(call root2dist,$(EMBOX_IMPORTED_CPPFLAGS))"'         >> $@
	@echo EMBOX_IMPORTED_CFLAGS='"$(call root2dist,$(EMBOX_IMPORTED_CFLAGS))"'             >> $@
	@echo EMBOX_IMPORTED_CXXFLAGS='"$(call root2dist,$(EMBOX_IMPORTED_CXXFLAGS))"'         >> $@
	@echo EMBOX_IMPORTED_LDFLAGS='"$(call root2dist,$(EMBOX_IMPORTED_LDFLAGS))"'           >> $@
	@echo EMBOX_IMPORTED_LDFLAGS_FULL='"$(call root2dist,$(EMBOX_IMPORTED_LDFLAGS_FULL))"' >> $@

TOOLCHAIN_TEST_SRC := $(ROOT_DIR)/mk/extbld/toolchain_test.c
TOOLCHAIN_TEST_OUT := $(OBJ_DIR)/toolchain_test

include $(EXTBLD_LIB)
include $(SRCGEN_DIR)/build.mk

.PHONY : do_test
do_test : $(TOOLCHAIN_TEST_OUT)

$(TOOLCHAIN_TEST_OUT): $(EMBOX_GCC_ENV)
ifeq ($(filter usermode%,$(ARCH)),)
	EMBOX_GCC_LINK=full $(EMBOX_GCC) $(TOOLCHAIN_TEST_SRC) -o $(TOOLCHAIN_TEST_OUT)
else
	@echo "Full linking mode isn't supported for usermode arch!"
endif


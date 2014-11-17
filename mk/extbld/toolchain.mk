
include $(MKGEN_DIR)/build.mk
include mk/flags.mk
include $(SRCGEN_DIR)/image.rule.mk

rootdir := $(abspath $(ROOT_DIR))
EMBOX_IMPORTED_CPPFLAGS := $(filter -D% -U% -I% -nostdinc,$(filter-out -D"% -D'%,$(EMBOX_EXPORT_CPPFLAGS)))

EMBOX_IMPORTED_CFLAGS   := $(filter -g% -f% -m% -O% -G% -E%,$(CFLAGS))
EMBOX_IMPORTED_CXXFLAGS := $(filter -g% -f% -m% -O% -G% -E%,$(CXXFLAGS))

EMBOX_IMPORTED_LDFLAGS  := $(filter -static -nostdlib -E%,$(LDFLAGS))
EMBOX_IMPORTED_LDFLAGS  += $(foreach w,$(filter -m elf_i386,$(LDFLAGS)),-Wl,$w)
ifeq ($(ARCH),microblaze)
# microblaze compiler wants vendor's xillinx.ld if no lds provided from command line.
# Make it happy with empty lds
_empty_lds_hack:=$(abspath $(SRCGEN_DIR))/empty.lds
$(shell touch $(_empty_lds_hack))
EMBOX_IMPORTED_LDFLAGS += -T $(_empty_lds_hack)
endif

EMBOX_IMPORTED_LDFLAGS_FULL :=
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--relax
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,-T,$(rootdir)/build/base/obj/mk/image.lds
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=__symbol_table=0,--defsym=__symbol_table_size=0
EMBOX_IMPORTED_LDFLAGS_FULL += $(rootdir)/build/base/obj/embox.o
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--start-group
EMBOX_IMPORTED_LDFLAGS_FULL += $(__image_ld_libs1:.%=-Wl,$(rootdir)%)
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--end-group

# We are not including user ld scripts in flags, since it is not required to do
# working object, only compilation errors are matter. If required, place it below
# --relax flag and perfrom 2 phase link at arch-unknown-embox, first with user lds,
# next with image.lds
#EMBOX_IMPORTED_LDFLAGS_FULL += $(__image_ld_scripts1:.%=-Wl,-T,$(rootdir)%)

EMBOX_IMPORTED_MAKEFLAGS :=
ifneq (,$(filter -j,$(MAKEFLAGS)))
EMBOX_IMPORTED_MAKEFLAGS += -j $(shell nproc)
endif

$(EMBOX_GCC_ENV): $(MKGEN_DIR)/build.mk mk/flags.mk $(SRCGEN_DIR)/image.rule.mk
$(EMBOX_GCC_ENV): | $(dir $(EMBOX_GCC_ENV))
	@echo EMBOX_CROSS_COMPILE="'"$(CROSS_COMPILE)"'"                       >> $@
	@echo EMBOX_IMPORTED_CPPFLAGS="'"$(EMBOX_IMPORTED_CPPFLAGS)"'"         >> $@
	@echo EMBOX_IMPORTED_CFLAGS="'"$(EMBOX_IMPORTED_CFLAGS)"'"             >> $@
	@echo EMBOX_IMPORTED_CXXFLAGS="'"$(EMBOX_IMPORTED_CXXFLAGS)"'"         >> $@
	@echo EMBOX_IMPORTED_LDFLAGS="'"$(EMBOX_IMPORTED_LDFLAGS)"'"           >> $@
	@echo EMBOX_IMPORTED_LDFLAGS_FULL="'"$(EMBOX_IMPORTED_LDFLAGS_FULL)"'" >> $@

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


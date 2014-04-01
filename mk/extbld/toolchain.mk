
.PHONY: $$(image_prerequisites)

include $(MKGEN_DIR)/build.mk
include mk/flags.mk
include $(SRCGEN_DIR)/image.rule.mk

EMBOX_IMPORTED_CPPFLAGS :=
EMBOX_IMPORTED_CPPFLAGS += -I$(shell pwd)/include
EMBOX_IMPORTED_CPPFLAGS += $(filter -I%,$(EMBOX_EXPORT_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -nostdinc,$(EMBOX_EXPORT_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -D__EMBOX__,$(EMBOX_EXPORT_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -Uarm,$(EMBOX_EXPORT_CPPFLAGS))

EMBOX_IMPORTED_CFLAGS :=
EMBOX_IMPORTED_CFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CFLAGS += $(filter -g,$(CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -fno-common,$(CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -fno-omit-frame-pointer,$(CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -fno-optimize-sibling-calls,$(CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -fno-stack-protector,$(CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -O%,$(CFLAGS))
# architecture dependent options
EMBOX_IMPORTED_CFLAGS += $(filter -m32,$(CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -mapcs,$(CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -march%,$(CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -marm,$(CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -mlittle-endian,$(CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -mno-thumb-interwork,$(CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -mno-unaligned-access,$(CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -msoft-float, $(CFLAGS))

EMBOX_IMPORTED_CXXFLAGS :=
EMBOX_IMPORTED_CXXFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CXXFLAGS += $(filter -g,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-common,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-exceptions,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-omit-frame-pointer,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-optimize-sibling-calls,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-stack-protector,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-threadsafe-statics,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -O%,$(CXXFLAGS))
# architecture dependent options
EMBOX_IMPORTED_CXXFLAGS += $(filter -m32,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mapcs,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -march%,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -marm,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mlittle-endian,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mno-thumb-interwork,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mno-unaligned-access,$(CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -msoft-float, $(CXXFLAGS))

EMBOX_IMPORTED_LDFLAGS :=
EMBOX_IMPORTED_LDFLAGS += $(filter -static,$(LDFLAGS))
EMBOX_IMPORTED_LDFLAGS += $(filter -nostdlib,$(LDFLAGS))
ifneq (,$(filter -m%,$(LDFLAGS)))
EMBOX_IMPORTED_LDFLAGS += -Wl,$(filter -m%,$(LDFLAGS))
endif

EMBOX_IMPORTED_LDFLAGS_FULL :=
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--relax
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=__symbol_table=0,--defsym=__symbol_table_size=0
#EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,-T,$(ROOT_DIR)/build/base/obj/mk/image.lds
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=_ram_base=0,--defsym=_ram_size=0
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=_traps_text_start=0,--defsym=_traps_text_end=0
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=_text_vma=0,--defsym=_text_lma=0,--defsym=_text_len=0
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=_rodata_vma=0,--defsym=_rodata_lma=0,--defsym=_rodata_len=0
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=_data_vma=0,--defsym=_data_lma=0,--defsym=_data_len=0
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=_bss_vma=0,--defsym=_bss_lma=0,--defsym=_bss_len=0
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=_ctors_start=0,--defsym=_ctors_end=0
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=_dtors_start=0,--defsym=_dtors_end=0
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=__EH_FRAME_BEGIN__=0
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=_bss_end=0
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=_reserve_end=0
EMBOX_IMPORTED_LDFLAGS_FULL += -Wl,--defsym=_bss_len=0,--defsym=_bss_len_with_reserve=0

EMBOX_IMPORTED_MAKEFLAGS :=
ifneq (,$(filter -j,$(MAKEFLAGS)))
EMBOX_IMPORTED_MAKEFLAGS += -j $(shell nproc)
endif

rootdir := $(abspath $(ROOT_DIR))

$(EMBOX_GCC_ENV): $(MKGEN_DIR)/build.mk mk/flags.mk $(SRCGEN_DIR)/image.rule.mk
$(EMBOX_GCC_ENV): _ld_objs    = $(__image_ld_objs1:.%=-Wl,$(rootdir)%)
$(EMBOX_GCC_ENV): _ld_libs    = $(__image_ld_libs1:.%=-Wl,$(rootdir)%)
$(EMBOX_GCC_ENV): _ld_scripts = $(__image_ld_scripts1:.%=-Wl,-T,$(rootdir)%)
$(EMBOX_GCC_ENV): | $(dir $(EMBOX_GCC_ENV))
	@echo "EMBOX_CROSS_COMPILE='$(CROSS_COMPILE)'"               >> $@
	@echo "EMBOX_IMPORTED_CPPFLAGS='$(EMBOX_IMPORTED_CPPFLAGS)'" >> $@
	@echo "EMBOX_IMPORTED_CFLAGS='$(EMBOX_IMPORTED_CFLAGS)'"     >> $@
	@echo "EMBOX_IMPORTED_CXXFLAGS='$(EMBOX_IMPORTED_CXXFLAGS)'" >> $@
	@echo "EMBOX_IMPORTED_LDFLAGS='$(EMBOX_IMPORTED_LDFLAGS)'"   >> $@
	@echo "EMBOX_IMPORTED_LDFLAGS_FULL='$(EMBOX_IMPORTED_LDFLAGS_FULL) \
		$(_ld_scripts) \
		$(_ld_objs) \
		-Wl,--start-group \
		$(_ld_libs) \
		-Wl,--end-group \
		'"   >> $@

$$(image_prerequisites):

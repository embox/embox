#
#   Date: May 14, 2021
# Author: Alexander Kalmuk
#

include mk/script/script-common.mk

fqn2id = $(subst .,__,$1)

module_ids := \
	$(foreach m,$(call get,$(build_model),modules), \
		$(call fqn2id,$(call get,$(call get,$m,type),qualifiedName)))

mods := \
	$(foreach m,$(call get,$(build_model),modules), \
		$(call get,$m,type))

# 1. Annotation target
# 2. Annotation option
annotation_value = $(call get,$(call invoke,$1,getAnnotationValuesOfOption,$2),value)

my_linker_section_text := $(call mybuild_resolve_or_die,mybuild.lang.LinkerSection.text)
my_linker_section_text_val = \
	$(strip $(call annotation_value,$1,$(my_linker_section_text)))

my_linker_section_rodata := $(call mybuild_resolve_or_die,mybuild.lang.LinkerSection.rodata)
my_linker_section_rodata_val = \
	$(strip $(call annotation_value,$1,$(my_linker_section_rodata)))

my_linker_section_data := $(call mybuild_resolve_or_die,mybuild.lang.LinkerSection.data)
my_linker_section_data_val = \
	$(strip $(call annotation_value,$1,$(my_linker_section_data)))

my_linker_section_bss := $(call mybuild_resolve_or_die,mybuild.lang.LinkerSection.bss)
my_linker_section_bss_val = \
	$(strip $(call annotation_value,$1,$(my_linker_section_bss)))

# Checks whether the app is in external memory.
mod_in_extmem = \
	$(and $(call my_linker_section_$2_val,$1), \
		$(shell grep LDS_SECTION_VMA_$(call my_linker_section_$2_val,$1) \
			$(SRCGEN_DIR)/config.lds.h) \
	)

__prefix = \
	__module_$(strip $(call fqn2id,$(call get,$1,qualifiedName)))_$2

define section_symbols
__SECTION_SYMBOLS($(call __prefix,$1,$2),.$(call my_linker_section_$2_val,$1))
endef

print_symbols = \
	$(foreach m,$(mods), \
		$(if $(call mod_in_extmem,$m,$1), \
			$(info $(call section_symbols,$m,$1)),) \
	)

$(foreach s,text rodata data bss, \
	$(info /* $s */) \
	$(call print_symbols,$s) \
)

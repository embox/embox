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

mod_symbol_name = \
	__module_$(strip $(call fqn2id,$(call get,$1,qualifiedName)))_$2_$3

define extern_syms
extern char $(call mod_symbol_name,$1,$2,vma);
extern char $(call mod_symbol_name,$1,$2,lma);
extern char $(call mod_symbol_name,$1,$2,len);
endef

define array_start_vma
void *sections_$1_$2[] = {
endef
define array_end_vma
	(void *)0
};
endef

define array_start_lma
void *sections_$1_$2[] = {
endef
define array_end_lma
	(void *)0
};
endef

define array_start_len
unsigned sections_$1_$2[] = {
endef
define array_end_len
	(unsigned)-1
};
endef

define elem_vma
	(void *)&$(call mod_symbol_name,$1,$2,vma),
endef

define elem_lma
	(void *)&$(call mod_symbol_name,$1,$2,lma),
endef

define elem_len
	(unsigned)&$(call mod_symbol_name,$1,$2,len),
endef

print_extern_ref = \
	$(foreach m,$(mods), \
		$(if $(call mod_in_extmem,$m,$1), \
			$(info $(call extern_syms,$m,$1)),) \
	)

print_section_array = \
	$(info $(call array_start_$2,$1,$2)) \
	$(foreach m,$(mods), \
		$(if $(call mod_in_extmem,$m,$1), \
			$(info $(call elem_$2,$m,$1)),) \
	) \
	$(info $(call array_end_$2))

$(foreach s,text rodata data bss, \
	$(info /* $s */) \
	$(call print_extern_ref,$s) \
)

$(foreach s,text rodata data bss, \
	$(info /* $s */) \
	$(foreach i,vma lma len, $(call print_section_array,$s,$i)) \
)

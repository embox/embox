#
#   Date: Sep 27, 2012
# Author: Eldar Abusalimov
#

include mk/script/script-common.mk
include mk/script/build-deps.mk

my_app := $(call mybuild_resolve_or_die,mybuild.lang.App)

is_app = \
	$(strip $(call invoke, \
		$(call get,$1,allTypes),getAnnotationsOfType,$(my_app)))

fqn2id = $(subst .,__,$1)

modules := \
	$(foreach m,$(call get,$(build_model),modules), \
		$(call get,$m,type))

apps := \
	$(foreach m,$(call get,$(build_model),modules), \
		$(if $(call is_app,$m), \
			$(call get,$m,type)))

# All apps with their @BuildDepends()'s modules. So if the app depends
# on another module with @BuildDepends it will be included in the list.
apps := \
	$(sort \
		$(foreach m,$(apps),$m $(foreach d,$(call build_deps_all,$m),$d)) \
	)

noapps := \
	$(filter-out $(apps),$(modules))

id = \
	$(call fqn2id,$(call get,$1,qualifiedName))

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

#
# The output.
#

\h := \\#

#XXX had to add ABSOLUTE(. - ABSOLUTE(...)) for old ld; maybe replace
# _len -> _end ?
define file_header
/* $(GEN_BANNER) */

#include <asm-generic/embox.lds.h>

#define __MODULE_ENRTY(section_, module_) $(\h)
		__module_##module_##_##section_##_vma = .; $(\h)
		*(.section_.module.module_); $(\h)
		*(.section_.*.module.module_); $(\h)
		__module_##module_##_##section_##_len = ABSOLUTE(. - $(\h)
			ABSOLUTE(__module_##module_##_##section_##_vma));

endef

define section_item
		__MODULE_ENRTY($1,$2)
endef
# __module_$2_$1_vma = .;
# *(.$2.$1)
# __module_$2_$1_len = ABSOLUTE(. - __module_$2_$1_vma);

define section_header
#define LDS_$1 $(\h)
		_$(shell echo $1 | tr A-Z a-z)_start = .; $(\h)
endef

define section_footer
		_$(shell echo $1 | tr A-Z a-z)_end = .;

endef

define app_data_reserve_bss
/* Reserves memory to copy data. */
#define LDS_APP_DATA_RESERVE_BSS $(\h)
		_app_reserve_vma = .; $(\h)
		. += _apps_data_end - _apps_data_start + 4;

endef

define lma_header
#define LDS_MODULES_DATA_LMA $(\h)
endef

# It's made in two expressions instead of a single one,
# because older ld (2.26.1) generates something wrong
# in the first case.
define lma_item
		__module_$1_data_lma = _data_lma - _data_vma; $(\h)
		__module_$1_data_lma += __module_$1_data_vma;
endef

print_lma = \
	$(info $(call lma_header)) \
	$(foreach n,$1, \
		$(if $(call mod_in_extmem,$n,data),, \
			 $(info $(call lma_item,$(call id,$n)) $(\h))) \
	) \
	$(info )

# 1. list of module ids
# 2. section name
# 3. optional section alignment
# 4. optional section suffix
print_section = \
	$(info $(call section_header,$(value 4))) \
	$(foreach n,$1, \
		$(if $(call mod_in_extmem,$n,$2),, \
				 $(info $(call section_item,$2,$(call id,$n)) $(\h))) \
	) \
	$(info $(call section_footer,$(value 4)))

$(info $(file_header))

$(call print_section,$(apps),data,,APPS_DATA)
$(call print_section,$(apps),bss,,APPS_BSS)
$(info $(app_data_reserve_bss))

$(call print_section,$(modules),text,DEFAULT_TEXT_ALIGNMENT,MODULES_TEXT)
$(call print_section,$(modules),rodata,,MODULES_RODATA)
$(call print_section,$(noapps),data,,MODULES_DATA)
$(call print_section,$(noapps),bss,,MODULES_BSS)

$(call print_lma,$(modules))

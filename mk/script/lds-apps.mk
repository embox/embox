#
#   Date: Sep 27, 2012
# Author: Eldar Abusalimov
#

include mk/script/script-common.mk


my_app := $(call mybuild_resolve_or_die,mybuild.lang.App)

is_app = \
	$(strip $(call invoke, \
		$(call get,$1,allTypes),getAnnotationsOfType,$(my_app)))

fqn2id = $(subst .,__,$1)

module_ids := \
	$(foreach m,$(call get,$(build_model),modules), \
		$(call fqn2id,$(call get,$(call get,$m,type),qualifiedName)))

app_ids := \
	$(foreach m,$(call get,$(build_model),modules), \
		$(if $(call is_app,$m), \
			$(call fqn2id,$(call get,$(call get,$m,type),qualifiedName))))

noapp_ids := \
	$(filter-out $(app_ids),$(module_ids))

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

# 1. list of module ids
# 2. section name
# 3. optional section alignment
# 4. optional section suffix
print_section = \
	$(info $(call section_header,$(value 4))) \
	$(foreach n,$1,$(info $(call section_item,$2,$n) $(\h))) \
	$(info $(call section_footer,$(value 4))) \

$(info $(file_header))

$(call print_section,$(app_ids),data,,APPS_DATA)
$(call print_section,$(app_ids),bss,,APPS_BSS)
$(info $(app_data_reserve_bss))

$(call print_section,$(module_ids),text,DEFAULT_TEXT_ALIGNMENT,MODULES_TEXT)
$(call print_section,$(module_ids),rodata,,MODULES_RODATA)
$(call print_section,$(noapp_ids),data,,MODULES_DATA)
$(call print_section,$(noapp_ids),bss,,MODULES_BSS)

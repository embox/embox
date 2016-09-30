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

#define MODULE_ENRTY(section_, module_) $(\h)
		__module_##module_##_##section_##_vma = .; $(\h)
		*(.section_.module.module_); $(\h)
		__module_##module_##_##section_##_len = ABSOLUTE(. - $(\h)
			ABSOLUTE(__module_##module_##_##section_##_vma));

SECTIONS {
endef

define section_item
		MODULE_ENRTY($1,$2)
endef
# __module_$2_$1_vma = .;
# *(.$2.$1)
# __module_$2_$1_len = ABSOLUTE(. - __module_$2_$1_vma);

section_header = $(\t).$1$(value 3) : \
		ALIGN($(or $(value 2),DEFAULT_DATA_ALIGNMENT)) {
section_footer = $(\t)$(\t)*(.$1) } /* .$1$(value 3) */

define file_footer

	.bss..reserve.apps (NOLOAD) : ALIGN(DEFAULT_DATA_ALIGNMENT) {
		/* MAX is a workaround to avoid PROGBITS set on empty section. */
		/* . += MAX(SIZEOF(.data.apps), 1); */
		/* MAX isn't avaible on old ld, at least at 2.20 */
		. += SIZEOF(.data.apps) + 4;
	}
	_app_data_vma = ADDR(.data.apps);
	_app_reserve_vma = ADDR(.bss..reserve.apps);
}
endef


# 1. list of module ids
# 2. section name
# 3. optional section alignment
# 4. optional section suffix
print_section = \
	$(info $(call section_header,$2,$(value 3),$(value 4))) \
	$(foreach n,$1,$(info $(call section_item,$2,$n))) \
	$(info $(call section_footer,$2,$(value 3),$(value 4)))

$(info $(file_header))

$(call print_section,$(app_ids),data,,.apps)
$(call print_section,$(app_ids),bss,,.apps)

$(call print_section,$(module_ids),text,DEFAULT_TEXT_ALIGNMENT)
$(call print_section,$(module_ids),rodata)
$(call print_section,$(noapp_ids),data)
$(call print_section,$(noapp_ids),bss)

$(info $(file_footer))


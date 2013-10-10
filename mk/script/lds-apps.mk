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

app_ids := \
	$(foreach m,$(call get,$(build_model),modules), \
		$(if $(call is_app,$m), \
			$(call fqn2id,$(call get,$(call get,$m,type),qualifiedName))))

#
# The output.
#

define file_header
/* $(GEN_BANNER) */

#include <asm-generic/embox.lds.h>

SECTIONS {
endef

define section_item
		__app_$2_$1_vma = .;
		*(.app.$2.$1)
		__app_$2_$1_len = ABSOLUTE(. - __app_$2_$1_vma);

endef

data_header = $(\t).data.apps : ALIGN(DATA_ALIGNMENT) {
data_footer = $(\t)}

define bss_header
	.bss..reserve.apps (NOLOAD) : ALIGN(DATA_ALIGNMENT) {
		/* MAX is a workaround to avoid PROGBITS set on empty section. */
		. += MAX(SIZEOF(.data.apps), 1);
endef

define bss_footer
	}
	_app_data_reserve_offset = ADDR(.bss..reserve.apps) - ADDR(.data.apps);
endef

define file_footer
}
endef


$(info $(file_header))

print_section = \
	$(info $($1_header)) \
	$(foreach n,$(app_ids),$(info $(call section_item,$1,$n))) \
	$(info $($1_footer))

$(call print_section,data)
$(call print_section,bss)

$(info $(file_footer))



ifndef _mybuild_check_mk_
_codegen_di_mk_ := 1

include mk/core/define.mk
include mk/mybuild/resource.mk
include mk/conf/roots.mk

# Param:
#   1. module name
#   2. module list
# Return:
#   Module descedant from list
define find_descedant_obj
	$(strip $(for o <- $1,
		m <- $2,
		$(with $m,
			$(for l <- $(get $1.super_module_ref),
				super <- $(invoke l->get_reference),
				$(if $(eq $(suffix $(super)),$(suffix $o)),
					$m,$(call $0,$(super)))
			)
		)
	))
endef

define find_descedant
	$(call find_descedant_obj,$(call find_mod,$1),$2)
endef

$(call def,find_descedant)
$(call def,find_descedant_obj)

endif


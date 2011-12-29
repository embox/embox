ifndef __mybuild_resource_mk
__mybuild_resource_mk := 1
include mk/core/string.mk
include mk/core/object.mk

define class-resource
	$(field nodes : node)
	$(field issues)
	$(property-field exports... : node)
	$(property-field my_file : node)
endef

#param $1 object contains resources
define get_modules
	$(get $1.modules)
endef

define set_exports
	$(set $1.exports,$(foreach o,$(call get_modules,$2),
		$(call get_qualified_name,$o)$o)
	)
endef



define resolve_internal
	$(foreach o,$(call get_modules,$1),
		$(foreach l,$(get $o.depends_refs),
			$(foreach i,$(call get_modules,$1),
				$(info $i::$(get $i.name):$l:$(get $l.link_name))
				$(if $(eq $(get $i.name),$(get $l.link_name)),
					$(set $l.link_name,$(get $i.name)$i))
			)
		)
	)
endef

define create_from_model
	$(info +++++++++++++++++++++++++++$1)
	$(if $(instance-of $1,my_file),
		$(foreach r,$(new resource),$r
			$(call set_exports,$r,$1)
			$(set $r.my_file,$1)
			$(set $1.resource,$r)
		)
	)
endef

$(def_all)

endif #__mybuild_resource_mk

ifndef __mybuild_resource_mk
__mybuild_resource_mk := 1
include mk/core/string.mk
include mk/core/object.mk

define class-resource
	$(field nodes : node)
	$(field issues)

	$(property-field exports... : node)
	#param $1 a model
	$(setter exports,
		$(set-field exports,
			$(foreach o,$1,$(call get_qualified_name,$o)$o)
		)
	)

	$(property-field my_file : node)

endef

# TODO move from here
# this function tries to resolve references which place in the same madel
# param $1 a model
define resolve_internal
	$(silent-foreach o,$(get $1.modules),
		$(foreach l,$(get $o.depends_refs),
			$(foreach i,$(get $1.modules),
				$(if $(eq $(get $i.name),$(get $l.link_name)),
					$(set $l.link_name,$(get $i.name)$i))
			)
		)
	)
endef

#create resource from associated model
#param $1 -a model
define create_from_model
	$(if $(instance-of $1,my_file),
		$(call resolve_internal,$1)
		$(foreach r,$(new resource),$r
			$(set $r.exports,$(get $1.modules))
			$(set $r.my_file,$1)
			$(set $1.resource,$r)
		)
	)
endef

$(def_all)

endif #__mybuild_resource_mk

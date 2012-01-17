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

	$(field imports)
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

define resolve_link_from_resource
	$(foreach m,$(filter $(get $1.link_name).%,$(get $2.exports)),
		#$(info link_name is $(get $1.link_name), exports is $m)
		$(set $1.dst,$m)
		$(set $1.link_name,$m)
		#$(info now linkname is $(get $1.link_name))
	)
endef

define resolve_links_from_files
	$(foreach l,$(call get-instances-of,module_link),
		$(or $($l.dst),
			$(foreach f,$1,
				#$(info link is $l, file is $f, resource is $($f))
				$(call resolve_link_from_resource,$l,$($f)))
			$l))
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


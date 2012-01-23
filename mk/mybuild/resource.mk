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
	$(silent-for o<-$(get $1.modules),
	      f<-depends_refs super_module_ref,
	      l<-$(get $o.$f),
	      i<-$(get $1.modules),
			$(if $(eq $(get $i.name),$(get $l.link_name)),
				$(set $l.link_name,$(get $i.name)$i)
				$(invoke $l.resolve,$(get $i.name)$i))
	)
endef

# param $1 is name
# param $2 is resource
# output module object
define find_mod_in_res
	$(filter $1.%,$(get $2.exports))
endef

# param $1 is a link
# param $2 is a resource
# output is
# 	zero if no resolve was made
# 	obj otherwise
define resolve_link_from_resource
	$(foreach m,$(call find_mod_in_res,$(get $1.link_name),$2),
		$(invoke $1.resolve,$m)
	)
endef

# param $1 list of files
# output is normalized obj list
define resolve_links_from_files
	$(suffix $(call __resolve_links_from_files,$1))
endef

# param $1 list of files
# output is obj list
define __resolve_links_from_files
	$(foreach l,$(call get-instances-of,module_link),
		$(if $(invoke $l.resolved?),
			$l,
			$(silent-foreach f,$1,
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


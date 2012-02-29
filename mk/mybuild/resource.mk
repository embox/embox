ifndef __mybuild_resource_mk
__mybuild_resource_mk := 1
include mk/core/string.mk
include mk/core/object.mk

# Constructor args:
#   1. (Optional) Filename to load resource from.
define class-resource
	$(field nodes : node)
	$(field issues)

	$(property-field exports... : node)
	#param $1 a model
	$(setter exports,
		$(set-field exports,
			$(foreach o,$1,$(get o->qualified_name)$o)
		)
	)

	$(field imports)
	$(property-field my_file : node)

	$(property-field filename)

	#param $1 is filename
	$(method load,
		$(for myfile <- $(call gold_parse,myfile,$1),
			$(set filename,$1)
			$(call resolve_internal,$(myfile))
			$(set exports,$(get myfile->modules))
			$(set my_file,$(myfile))
			$(set myfile->resource,$(this))
		)
	)

	$(if $(value 1),$(invoke load,$1))
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
	$(for e <- $(get $2.exportedObjects),
		$(if $(eq $(get e->qualifiedName),$1),
			$e))
endef

$(def_all)

endif #__mybuild_resource_mk


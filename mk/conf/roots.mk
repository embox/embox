ifndef __conf_roots_mk
__conf_roots_mk := 1

include mk/core/define.mk
include mk/mybuild/mybuild.mk
include mk/util/graph.mk

# param $1 is name
# output module object
define find_mod
	$(for moduleName <- $1,
		$(or $(for module <-
					$(map-get $(get mybuild_model_instance->resourceSet)
						.exportedObjectsMap/$(moduleName)),
				$(moduleName).$(module)),
			$(warning Unresolved Module '$1')))
endef

# function used for getting module's dependent modules
# param 1 module object
define get_deps
	$(get $1.depends)
endef

# All dependencies of modules
# param $1 is list of modules
# output is closure list of given modules plus dependencies
define module_closure
	$(foreach m,
		$(call sort,$(suffix $(foreach m,$1,
			$(call graph_closure,$m,get_deps)))),
		$(get m->qualifiedName)$m
	)
endef

$(def_all)

endif


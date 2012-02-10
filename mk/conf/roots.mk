ifndef __conf_roots_mk
__conf_roots_mk := 1

include mk/core/define.mk
include mk/mybuild/resource.mk
include mk/mybuild/mybuild.mk

#param $1 is list of module names
define find_mods
	$(foreach m,$1,
		$(or $(strip $(call find_mod,$m)),$(warning Cant find $m)))
endef

# param $1 is name
# output module object
define find_mod
	$(foreach r,$(get mybuild_model_instance->resources),
		$(call find_mod_in_res,$1,$r))
endef

# function used for getting module's dependent modules
# param 1 module object
define get_deps_and_supers
	$(foreach module_link,$(get $1.depends_refs) $(get $1.super_module_ref),
		$(get $(module_link).dst))
endef

# All dependencies of modules
# param $1 is list of modules
# output is closure list of given modules plus dependencies
define module_closure
	$(foreach m,
		$(call sort,$(suffix $(foreach m,$1,
			$(call graph_closure,$m,get_deps_and_supers)))),
		$(get m->qualified_name)$m
	)
endef

$(def_all)

endif


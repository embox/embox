ifndef __conf_roots_mk
__conf_roots_mk := 1

include core/define.mk
include mybuild/resource.mk

#param $1 is list of module names
define find_mods
	$(foreach m,$1,
		$(or $(strip $(call find_mod,$m)),$(warning Cant find $m)))
endef

# param $1 is name
# output module object
define find_mod
	$(foreach r,$(call get-instances-of,resource),
		$(call find_mod_in_res,$1,$r))
endef

# function used for getting module's dependent modules
# param 1 module object
define get_dependecies
	$(foreach module_link,$(get $1.depends_refs),
		$(get $(module_link).dst))
endef

# All dependencies of modules
# param $1 is list of modules
# output is closure list of given modules plus dependencies
define module_closure
	$(call sort, \
		$(foreach m,$1,\
			$(call graph_closure,$m,get_dependecies)))
endef

$(def_all)

endif


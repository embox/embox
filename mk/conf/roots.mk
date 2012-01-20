ifndef __conf_roots_mk
__conf_roots_mk := 1

include core/define.mk
include mybuild/resource.mk

#param $1 is list of module names
define find_mods
	$(foreach m,$1,
		$(or $(strip $(call find_mod,$m)),$(info Cant find $m)))
endef

# param $1 is name
# output module object
define find_mod
	$(foreach r,$(call get-instances-of,resource),
		$(call find_mod_in_res,$1,$r))
endef

$(def_all)

endif


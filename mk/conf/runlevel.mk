ifndef __conf_runlevel_mk
__conf_runlevel_mk := 1

include mk/conf/roots.mk

#param $1 module object
define get_runlevel_name
	embox.framework.level_$(or $(RUNLEVEL-$1),2)
endef

#param $1 module object
define runlevel_setup
	$(foreach r,$(call find_mod,$(call get_runlevel_name,$1)),
		$(info $r)
	)
endef

define runlevels_setup
	$(info $(\n)$(\n))
	$(foreach m,$1,
		$(call runlevel_setup,$m)
	)
endef

$(def_all)

endif #__conf_runlevel_mk

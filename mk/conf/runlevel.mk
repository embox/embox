ifndef __conf_runlevel_mk
__conf_runlevel_mk := 1

include mk/conf/roots.mk

#param $1 module name
define get_runlevel_name
	embox.framework.level_$(or $(RUNLEVEL-$1),2)
endef

#param $1 module object
define runlevel_setup
	$(foreach r,$(call find_mod,$(call get_runlevel_name,$(call basename,$1))),
		$(foreach m,$1,
			$(warning add dependence $m to $r)
			$(foreach l,$(new module_link,$m),
				$(info module_link = $l)
				$(invoke l->resolve,$m)
				$(invoke m->add_depends_refs,$l)
			)
		)
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

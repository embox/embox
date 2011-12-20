ifndef __mybuild_resource_mk
__mybuild_resource_mk := 1

include mk/core/object.mk

define class-resource
	$(field nodes : node)
	$(field issues)
	$(field exports : node)
endef

#param $1 resource object
define get_modules
	$(get $1.modules)
endef

define set_exports
	$(set $1.exports,$(call get_modules,$2))
endef

$(def_all)

endif #__mybuild_resource_mk

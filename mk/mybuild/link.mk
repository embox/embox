ifndef __mybuild_link_mk
__mybuild_link_mk := 1

include mk/mybuild/model.mk

define class-module_link
	$(super module_ref,$1)

	$(super link)

	$(method get_reference,
		$(get dst))

	$(method get_type,module)

	$(method set_depends,
		$(set src,$1))

	$(method resolved?,
		$(get dst))

	# Args:
	#   1. Object
	$(method resolve,
		$(set dst,$1))

	# Args:
	#   1. Object, link to which must be deresolved
	$(method deresolve,
		$(set dst,$(\0)))

	$(if $(value 2),
		$(set dst,$2))
endef

$(def_all)

endif # __mybuild_link_mk

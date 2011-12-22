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

	$(method find_resolve,
		$(foreach o,$(globals),
			$(if $(subst $($(this).link_name),,$($(o).link_name)),
				,
				$(invoke resolve,$($(o).object)))))
	# Args:
	#   1. Object
	$(method resolve,
		$(info resolving $(this) with $1)$(invoke set_references,dst,$1))

	# Args:
	#   1. Object, link to which must be deresolved
	$(method deresolve,
		$(invoke remove_references,dst,$1))

endef

$(def_all)

endif # __mybuild_link_mk

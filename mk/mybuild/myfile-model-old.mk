#
# MyFile-specific object model.
#
#   Date: Dec 14, 2011
# Author: Eldar Abusalimov
#

include mk/mybuild/model.mk

# Constructor args:
#   1. Package name.
define class-my_file
	$(super node)
	$(super named,$1)

	$(property-field resolved)
	$(property-field imports...)
	$(property-field modules... : module)
	$(property-field resource : resource)

	$(method set_imports,
		$(set imports,$1))

	$(method set_modules,
		$(invoke set_references,modules,$(suffix $1)))

	$(method add_modules,
		$(invoke add_references,modules,$(suffix $1)))

	$(method remove_modules,
		$(invoke remove_references,modules,$(suffix $1)))

#	$(method resolve_modules,
#		$(foreach m,$1,$(get $m.name)$m))
#
	$(method set_resolved,
		$(invoke set_references,resolved,
			$(foreach m,$1,$(get $m.name)$m)
		)
	)

endef

#	$(method resolve_modules,$1))
#	$(foreach m,$1,$m))$(get $m.name)

# Constructor args:
#   1. Module name.
define class-module
	$(super node)
	$(super named,$1)

	$(property-field modifiers...)
	#$(property-field super_module_ref : module_ref)
	$(property-field super_module_ref... : module_ref) #XXX ->
		#could be only one, but otherwise set_super_module_ref fails
		#	--Anton K

	$(method set_super_module_ref,
		$(invoke set_references,super_module_ref,$1))

	$(property-field depends_refs... : module_ref)

	$(method set_depends_refs,
		$(invoke clear_references,depends_refs)
		$(foreach dep,$1,
			$(invoke dep->set_depends,depends_refs$(this)) #XXX
			$(invoke add_references,depends_refs,$(dep))))

	$(property-field requires_refs...)
	$(property-field provides_refs...)

	$(property-field sources... : filename)
	$(property-field objects... : filename)

	$(method set_sources,
		$(invoke set_references,sources,$1))

	$(method set_objects,
		$(invoke set_references,objects,$1))

endef

# Constructor args:
#   1. (optional) value.
define class-string
	$(field value,$(value 1))
endef

# Constructor args:
#   1. (optional) value.
define class-filename
	$(super node)

	$(property-field name)
	$(setter name,
		$(if $(findstring $(\s),$(subst $(\t),$(\s),$(subst $(\n),$(\s),$1))),
			$(error \
					Invalid file name: '$1')
		)
		$(set-field name,$1)
	)

	# Construct.
	$(if $(value 1),$(set name,$1))
endef

# Constructor args:
#   1. Name representing the crosslink.
define class-module_ref
	$(super abstract_ref,$1)
	$(method get_link_type,module)

	$(method set_depends)
endef


$(def_all)


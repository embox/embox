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

	$(field imports)
	$(field modules : module)

	$(method set_imports,
		$(set imports,$1))

	$(method set_modules,
		$(invoke replace_refs,modules,$1))

	$(method add_modules,
		$(invoke add_to,modules,$1))

	$(method remove_modules,
		$(invoke remove_from,modules,$1))

endef

# Constructor args:
#   1. Module name.
define class-module
	$(super node)
	$(super named,$1)

	$(field modifiers)
	$(field super_module_ref : module_ref)

	$(method set_super_module_ref,
		$(invoke replace_refs,super_module_ref,$1))

	$(field depends_refs : module_ref)

	$(method set_depends_refs,
		$(invoke replace_refs,depends_refs,$1))

	$(field requires_refs)
	$(field provides_refs)

	$(field sources : filename)
	$(field objects : filename)

	$(method set_sources,
		$(invoke replace_refs,sources,$1))

	$(method set_objects,
		$(invoke replace_refs,objects,$1))

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

	$(field name)
	$(setter name,
		$(if $(findstring $(\s),$(subst $(\t),$(\s),$(subst $(\n),$(\s),$1))),
			$(error \
					Invalid file name: '$1')
		)
		$1
	)

	# Construct.
	$(if $(value 1),$(set name,$1))
endef

# Constructor args:
#   1. Name representing the crosslink.
define class-module_ref
	$(super abstract_ref,$1)
	$(method get_link_type,module)
endef

$(def_all)

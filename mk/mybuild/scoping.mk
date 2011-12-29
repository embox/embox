#
#   Date: Dec 12, 2011
# Author: Eldar Abusalimov
#

ifndef __mybuild_scoping_mk
__mybuild_scoping_mk := 1

# A scope defines which elements can be reached from a certain point within
# a model.
#
# Each element of a scope is a pair 'name/object'.
define class-scope
	# 1. Qualified name.
	$(method get_elements)

	# 1. Qualified name.
	$(method get_single_element)
endef

define class-null_scope
	$(super scope)
	$(method get_elements,)
	$(method get_single_element,)
endef
$(call def,class-null_scope)

null_scope_instance := $(call new,null_scope)

# Constructor args:
#   1. Parent scope.
define class-abstract_scope
	$(super scope)

	$(field parent_scope : scope,$(or $1,$(null_scope_instance)))

	$(assert $(instance-of $(get parent_scope),scope),
			Invalid parent scope: '$(get parent_scope)')

	# 1. Qualified name.
	$(method get_elements,# Override.
		$(invoke get_local_elements,$1) \
		$(invoke $(get parent_scope).get_elements)
	)

	# 1. Qualified name.
	$(method get_single_element,# Override.
		$(firstword $(or \
			$(invoke get_local_elements,$1),
			$(invoke $(get parent_scope).get_elements,$1)
		))
	)

	# 1. Qualified name.
	$(method get_local_elements,# Override.
		$(filter $1/%,$(invoke get_all_local_elements))
	)

	$(method get_all_local_elements)
endef

# Constructor args:
#   1. Parent scope.
#   2. Target node.
define class-node_scope
	$(super abstract_scope,$1)

	$(field target_node,$2)

	$(assert $(call instance_of,$(get target_node),node),
			Invalid target node reference: '$(get target_node)')

	$(method get_all_local_elements,# Override.
		$(foreach child,$(invoke get_element_children),
#			$()
		)
	)
endef

# Constructor args:
#   1. Parent scope.
define class-import_scope
	$(super abstract_scope,$1)

	$(field imported_entities)
	$(field imported_features)

	# 1. Entity FQN with optional wildcard (.% suffix).
	$(method add_imported_entity,
		$(set+ imported_entities,$1)
	)

	# 1. Feature FQN with optional wildcard (.% suffix).
	$(method add_imported_feature,
		$(set+ imported_features,$1)
	)
endef

$(def_all)

endif # __mybuild_scoping_mk


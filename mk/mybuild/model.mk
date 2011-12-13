#
# Base model framework.
#
#   Date: Dec 7, 2011
# Author: Eldar Abusalimov
#

ifndef __mybuild_model_mk
__mybuild_model_mk := 1

include mk/core/define.mk
include mk/core/object.mk

include mk/mybuild/scoping.mk

# A node can have a parent and children.
define class-node
	$(method get_parent_node)
	$(method set_parent_node)

	$(method get_child_nodes)
	$(method set_child_nodes)

endef

# Abstract node implementation.
define class-abstract_node
	$(field parent_node:*)
	$(field child_nodes:*)

	$(method get_parent_node,$(get parent_node))
	$(method set_parent_node,
		$(if $(eq $1,$(get parent_node)),
			,# Do nothing.
			$(assert $(if $1,$(singleword $1)),
					Assigning more than one parent: '$1')
			$(foreach old,$(get parent_node),$(set- old->child_nodes,$(this)))
			$(set parent_node,$1)
			$(foreach new,$1,$(set+ new->child_nodes,$(this)))
		)
	)

	$(method get_child_nodes,$(get child_nodes))
	$(method set_child_nodes,
		$(foreach old,$(filter-out $1,$(get child_nodes)),
				$(invoke old->set_parent_node,))
		$(foreach new,$(filter-out $(get child_nodes),$1),
				$(invoke this->set_parent_node,$(this)))
	)

endef

define class-named
	$(method get_name)
	$(method set_name)
endef

# Constructor args:
#   1. Name.
define class-abstract_named
	$(field name,$(value 1))

	$(method get_name,$(get name))
	$(method set_name,$(set name,$1))
endef

define class-resource
	$(field nodes)
	$(field issues)
endef

# Constructor args:
#   1. Resource.
define class-list
	$(super node,$1)

	# Adds the given elements to this list.
	#   1. Elements to add.
	# Return:
	#   Element that were newly added.
	$(method add,
		$(and $(foreach element,$1,
			$(invoke node_containment_helper->set_container_of,
					$(element),$(this))
		),)
	)

	# Removes the given elements from this list.
	# Objects that don't belong to this list are not touched.
	#   1. Elements to remove.
	# Return:
	#   Element that were actually removed.
	$(method remove,
		$(and $(foreach element,$(filter $(get children),$1),
			$(invoke node_containment_helper->set_container_of,
					$(element),)
		),)
	)

	$(method get_elements,
		$(get children)
	)
endef

# Constructor args:
#   1. Package name.
define class-model
	$(super abstract_node)
	$(super abstract_named,$1)

	$(field imports)
	$(field modules:module)

	$(method set_modules,
		$(set modules,$1)
	)
endef

# Constructor args:
#   1. Module name.
define class-module
	$(super abstract_node)
	$(super abstract_named,$1)

	$(field modifiers)
	$(field super_module_ref : module_ref)

	$(field depends_refs : module_ref)

	$(field requires_refs)
	$(field provides_refs)

	$(field sources : filename)
	$(field objects : filename)
endef

# Constructor args:
#   1. (optional) value.
define class-string
	$(field value,$(value 1))
endef

# Constructor args:
#   1. (optional) value.
define class-filename
	$(field name)
	$(setter name,
		$(if $(findstring $(\s),$(subst $(\t),$(\s),$(subst $(\n),$(\s),$1))),
			$(error \
					Invalid file name: '$1')
		)
	)

	# Construct.
	$(if $(value 1),$(set name,$1))
endef

# Constructor args:
#   1. Name representing the crosslink.
define class-abstract_ref
	$(field src : *)
	$(field dst : *)

	$(field link_name,$1)

	$(method get_link_name,$(get link_name))

	$(method get_link_type)
endef

# Constructor args:
#   1. Name representing the crosslink.
define class-module_ref
	$(super abstract_ref,$1)
	$(method get_link_type,module)
endef

$(def_all)

endif # __mybuild_model_mk

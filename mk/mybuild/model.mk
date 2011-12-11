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
	$(field src)
		$(setter src,
			$(assert $(if $1,$(call is_object,$1)))$1)
	$(field dst)
		$(setter dst,
			$(assert $(if $1,$(call is_object,$1)))$1)

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

# Constructor args:
#   1. Module name.
define class-module
	$(field name,$1)

	$(field modifiers)
	$(field super_module_ref)

	$(field depends_refs)

	$(field requires_refs)
	$(field provides_refs)

	$(field sources)
	$(field objects)
endef

# Constructor args:
#   1. Name of property in each contained object.
#   2. Name of property in container.
define class-containment_helper
	$(field prop_in_contents,$(or $(singleword $1),
			$(error 'prop_in_contents' must be a singleword: '$1')))
	$(setter prop_in_contents,$(error $0 is read-only))

	$(field prop_in_container,$(or $(singleword $2),
			$(error 'prop_in_container' must be a singleword: '$2')))
	$(setter prop_in_container,$(error $0 is read-only))

	# 1. Contained (child) node.
	# 2. New container (parent).
	$(method set_container_of,
		$(foreach parent,$(get prop_in_contents),
				$(foreach children,$(get prop_in_container),
			$(or $(eq $2,$(get $1.$(parent))),
				$(foreach old,$(get $1.$(parent)),$(set- old->$(children),$1))
				$(set $1.$(parent),$2)
				$(foreach new,$2,$(set+ new->$(children),$1))
			)
		))
	)

endef

create_containment_helper = $(new containment_helper,$1,$2)

# Constructor args:
#   1. Resource.
define class-node
	$(field resource)
	$(invoke resource_containment_helper->set_container_of,$(this),$1)

	$(field children)
	$(field parent)

	# Returns a list of referenced objects.
	$(method get_references,
		$(sort $(foreach f,$(invoke get_reference_features),$(get $f)))
	)

	# Implementation have to return a list of names of fields corresponding
	# to reference features.
	$(method get_reference_features)# Abstract.

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
#   1. Resource.
define class-model
	$(super node,$1)
endef

$(def_all)

resource_containment_helper := $(call create_containment_helper,resource,nodes)
node_containment_helper := $(call create_containment_helper,parent,children)

endif # __mybuild_model_mk

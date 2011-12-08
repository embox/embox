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
#   1. Name of property in container.
#   1. Name of property in each contained object.
define class-containment
	$(field property_in_container,$(or $(singleword $1),
			$(error 'property_in_container' must be a singleword: '$1')))
	$(setter property_in_container,$(error $0 is read-only))

	$(field property_in_contents,$(or $(singleword $2),
			$(error 'property_in_contents' must be a singleword: '$2')))
	$(setter property_in_contents,$(error $0 is read-only))

	$(field container)
	$(setter container,
		$(or $(eq $1,$(get container)),
			$(and $(foreach p,$(get property_in_contents),
				$(foreach c,$(get container),$(set- $c.$p,$(this)))
				$(if $1,$(set+ $1.$p,$(this)))
			),)
			$1
		)
	)

	$(field contents)
	$(setter contents,
		$(and $(foreach p,$(get property_in_container),
			$(foreach c,$(get contents),$(set- $c.$p,$(this)))
			$(foreach 1,$1,$(set+ $1.$p,$(this)))
		),)
		$1
	)
endef

# Constructor args:
#   1. Resource.
define class-my_object
	$(field resource)$(new containment,objects,resource)

	$(field container)
	$(setter container,
		$(or $(eq $1,$(get container)),
			$(foreach c,$(get container),$(set- $c.contents,$(this)))
			$(if $1,$(set+ $1.contents,$(this)))
			$1
		)
	)
	$(field contents)

	# Returns a list of referenced objects.
	$(method get_references,
		$(sort $(foreach f,$(invoke get_reference_features),$(get $f)))
	)

	# Implementation have to return a list of names of fields corresponding
	# to reference features.
	$(method get_reference_features)# Abstract.

endef

define class-my_resource
	$(field objects)
	$(field issues)
endef

$(def_all)

endif # __mybuild_model_mk

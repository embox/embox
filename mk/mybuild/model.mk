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
#   1. Resource.
define class-my_object
	$(super proxy,# Object is resolved.
		)

	$(field resource,
		$1)

	$(field container)
	$(field contents)

	# Returns a list of referenced objects.
	$(method get_references,
		$(sort $(foreach f,$(invoke get_reference_features),$(get $f)))
	)

	# Implementation have to return a list of names of fields corresponding
	# to reference features.
	$(method get_reference_features)# Abstract.

endef

# Constructor args:
#   1. Proxy name.
define class-my_proxy
	# Name for unresolved object, empty otherwise.
	$(field proxy,$1)
endef

define class-my_resource
	$(field objects)
	$(field issues)
endef

$(def_all)

endif # __mybuild_model_mk

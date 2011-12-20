#
# Model framework core interfaces.
#
#   Date: Dec 20, 2011
# Author: Eldar Abusalimov
#

ifndef __model_model_mk
__model_model_mk := 1

include mk/core/define.mk
include mk/core/object.mk

#
# Interface of all modelled objects.
#
define class-Node

	# Returns meta class of the instance.
	$(method getMetaClass)

	# 1. Meta reference.
	$(method getNodeReference)

	# 1. Meta reference.
	# 2. New references.
	$(method setNodeReference)

	# 1. Meta reference.
	# 2. What to add.
	$(method addNodeReference)

	# 1. Meta reference.
	# 2. What to remove.
	$(method removeNodeReference)

	# 1. Meta reference.
	$(method clearNodeReference)

endef

#
# Named proxy of referenced node.
#
define class-Reference

	# Returns meta reference represented by this proxy.
	$(method getMetaReference)

	# Returns meta class of referenced object.
	$(method getReferenceMetaClass)

	# Returns a textual representation of the reference.
	$(method getReferenceString)

	# Sets a new value of 'referenceString' property.
	$(method setReferenceString)

endef

$(def_all)

endif # __model_model_mk

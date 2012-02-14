#
#   Date: Jan 21, 2012
# Author: Eldar Abusalimov
#

ifndef __model_resource_mk
__model_resource_mk := 1

include mk/model/model.mk
include mk/model/metamodel.mk

include mk/model/linkage.mk

# Constructor args:
#   1. (optional) File to load the resource from.
define class-Resource
	$(super LinkageUnit)

	$(property fileName)
	$(field fileName)
	$(getter fileName,
		$(get-field fileName))

	# The root of the containment hierarchy of this resource.
	# Empty if the resource is not yet loaded.
	$(property-field rootObject : EObject)

	# List of named objects in the containment hierarchy.
	# Empty for not yet loaded resource.
	$(property exportedObjects... : ENamedObject)
	$(getter exportedObjects,
		$(for root <- $(get rootObject),
			child  <- $(root) $(get root->eAllContents),
			$(if $(invoke EModel_ENamedObject->isInstance,$(child)),$(child)))
	)

	# List of not yet resolved links.
	# Empty for not yet loaded resource.
	$(property unresolvedLinks... : ELink)
	$(getter unresolvedLinks,
		$(for root <- $(get rootObject),
			child  <- $(root) $(get root->eAllContents),
			link  <- $(get-field child->__eUnresolvedLinks),
			$(if $(not $(get link->eTarget)),
				$(link)))
	)

	$(property-field issues... : Issue)

	# Loads the resource from the specified location.
	#   1. File name.
	$(method load,
		$(set-field fileName,$1)
		$(set rootObject,$(invoke loadRootObject,$1)))

	# Implementation have to retrieve the root object of this resource.
	#   1. File name.
	# Return:
	#   Loaded root or empty on error.
	$(method loadRootObject)

	$(if $(value 1),
		$(invoke load,$1))
endef

# Constructor args:
#   1. Resource.
#   2. (optional) severity.
#   3. (optional) location.
#   4. (optional) message.
define class-Issue

	$(property-field resource : Resource,$1)

	$(property-field severity,$(value 2))
	$(property-field location,$(value 3))
	$(property-field message,$(value 4))

	$(method report,
		$(info $(get $(get resource).fileName):$(get location): \
			$(get severity): $(get message)))

endef

$(def_all)

endif # __model_resource_mk

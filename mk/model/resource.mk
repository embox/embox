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
	$(setter rootObject,
		$(assert $(not $(multiword $1)))

		$(for oldRootObject <- $(get-field rootObject),
			$(set-field oldRootObject->__eContainer,))

		$(for newRootObject <- $1,
			$(assert $(not $(invoke EModel_ELink->isInstance,$(newRootObject))),
				ELink can't be set as 'rootObject' of resource)
			$(assert $(not $(get-field newRootObject->__eContainer)),
				Only the root of containment hierarchy \
				which does not belong to another resource \
				can be set as 'rootObject' of resource)
			$(set-field newRootObject->__eContainer,$(this)))

		$(set-field rootObject,$(suffix $1))
	)

	# List of named objects in the containment hierarchy.
	# Empty for not yet loaded resource.
	$(property exportedObjects... : ENamedObject)
	$(getter exportedObjects,
		$(for root <- $(get rootObject),
			child  <- $(root) $(invoke root->eAllContents),
			$(if $(invoke EModel_ENamedObject->isInstance,$(child)),$(child)))
	)

	# List of not yet resolved links.
	# Empty for not yet loaded resource.
	$(property unresolvedLinks... : ELink)
	$(getter unresolvedLinks,
		$(for root <- $(get rootObject),
			child  <- $(root) $(invoke root->eAllContents),
			link  <- $(invoke child->eUnresolvedLinks),
			$(if $(not $(invoke link->eTarget)),
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
define class-Issue
	$(property resource : Resource)

	$(property severity)
	$(property location)
	$(property message)

	$(property isError)
	$(getter isError,
		$(filter error,$(get severity)))

	$(property isWarning)
	$(getter isWarning,
		$(filter warning,$(get severity)))

	$(method report,
		$(with \
			$(get $(get resource).fileName):$(get location): \
			$(get severity): $(get message).,

			$(shell env echo '$(subst ','"'"',$1)' >&2)))

endef

# Constructor args:
#   1. Resource.
#   2. (optional) severity.
#   3. (optional) location.
#   4. (optional) message.
define class-BaseIssue
	$(super Issue)

	$(property-field resource : Resource,$1)

	$(property-field severity,$(value 2))
	$(property-field location,$(value 3))
	$(property-field message,$(value 4))

endef

# Constructor args:
#   1. Link.
define class-UnresolvedLinkIssue
	$(super Issue)

	$(property-field link : ELink,$1)

	$(property resource : Resource)
	$(getter resource,
		$(get $(get link).eResource))

	$(getter severity,
		error)
	$(getter location,
		0:0)
	$(getter message,
		$(for link     <- $(get link),
			targetType <- $(get $(get link->eMetaReference).eReferenceType),

			Couldn't resolve reference to $(get targetType->name) \
				'$(get link->name)'))

endef

$(def_all)

endif # __model_resource_mk

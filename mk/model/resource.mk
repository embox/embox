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
	$(property-field contentsRoot : EObject)
	$(setter contentsRoot,
		$(assert $(not $(multiword $1)))

		# Reset file name.
		$(set-field fileName,)

		$(for oldRootObject <- $(get-field contentsRoot),
			$(set-field oldRootObject->__eContainer,))

		$(for newRootObject <- $1,
			$(assert $(not $(invoke EModel_ELink->isInstance,$(newRootObject))),
				ELink can't be set as 'contentsRoot' of resource)
			$(assert $(not $(get-field newRootObject->__eContainer)),
				Only the root of containment hierarchy \
				which does not belong to another resource \
				can be set as 'contentsRoot' of resource)
			$(set-field newRootObject->__eContainer,$(this)))

		$(set-field contentsRoot,$(suffix $1))
	)

	$(property exportedObjects... : ENamedObject)
	$(getter exportedObjects,
		$(get namedContents))

	# List of all objects in the containment hierarchy.
	# Empty for not yet loaded resource.
	$(property contents... : EObject)
	$(getter contents,
		$(for root <- $(get contentsRoot),
			$(root) $(invoke root->eAllContents))
	)

	# List of named objects in the containment hierarchy.
	# Empty for not yet loaded resource.
	$(property namedContents... : ENamedObject)
	$(getter namedContents,
		$(for root <- $(get contentsRoot),
			$(if $(invoke EModel_ENamedObject->isInstance,$(root)),
				$(root) )
			$(invoke root->eAllContentsOfType,$(EModel_ENamedObject)))
	)

	# List of not yet resolved links.
	# Empty for not yet loaded resource.
	$(property unresolvedLinks... : ELink)
	$(getter unresolvedLinks,
		$(for child <- $(get contents),
			$(invoke child->eUnresolvedLinks))
	)

	$(property-field issues... : Issue)

	# Loads the resource from the specified location.
	#   1. File name.
	$(method load,
		$(set contentsRoot,$(invoke loadRootObject,$1))
		$(set-field fileName,$1))

	# Implementation have to retrieve the root object of this resource.
	#   1. File name.
	# Return:
	#   Loaded root or empty on error.
	$(method loadRootObject)

	$(if $(value 1),
		$(invoke load,$1))
endef

define class-ResourceSet

	$(property resources... : Resource)

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
		$(get $(get link).origin))
	$(getter message,
		$(for link     <- $(get link),
			targetType <- $(get $(get link->eMetaReference).eReferenceType),

			Couldn't resolve reference to $(get targetType->name) \
				'$(get link->name)'))

endef

$(def_all)

endif # __model_resource_mk

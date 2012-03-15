#
#   Date: Jan 22, 2012
# Author: Eldar Abusalimov
#

ifndef __model_linkage_mk
__model_linkage_mk := 1

include mk/model/model.mk
include mk/model/metamodel.mk

define class-Linker

	# Links all resources of the given resource set against each other.
	#   1. The resource set to link.
	$(method link,
		$(invoke linkAgainst,$1,$1))

	# Links the given resource set against another one.
	#   1. The resource set being linked.
	#   2. Resource set to search for exported objects.
	$(method linkAgainst,
		$(strip $(for \
			resourceSet <- $(suffix $1),
			sourceSet   <- $(suffix $2),
			resource    <- $(get resourceSet->resources),

			$(call Linker.resolveLinks,$(get resource->unresolvedLinks))
		))
	)

	$(method resolveLinks,
		$(for \
			link        <- $1,
			linkName    <- $(get link->name),
			linkPrefix  <- $(firstword $(subst ., ,$(linkName))),
			targetType  <- $(get $(get link->eMetaReference).eReferenceType),

			$(call Linker.doLink)))

	$(method doLink,
		$(with \
			$(or \
				$(invoke linkHandler,$(link)),
				$(invoke lookupContainerChain,$(invoke link->eSource)),
				$(invoke searchGlobalScopeUsingResourceImports),
				$(invoke searchGlobalScopeByFullName)),

			$(if $(singleword $1),
				$(invoke link->resolve,$1),

				$(set+ resource->issues,
					$(new UnresolvedLinkIssue,$(link))))

		)
	)

	# Args:
	#  1. Object linking
	$(method linkHandler,
		$(for obj <- $1,
			handleName <- linkHandle-$(basename $(get-field $1.eSource)),
			$(if $(has-method $(this),$(handleName)),
				$(invoke $(handleName),$1))))
	# Param:
	#   1. Link source.
	$(method lookupContainerChain,
		$(if $(and $1,$(invoke EModel_ENamedObject->isInstance,$1)),
			$(or $(call Linker.searchGlobalScopeWithNormalizers,
					$(invoke getLocalElementNormalizers,$1)),
				$(call Linker.searchGlobalScopeWithNormalizers,
					$(invoke getLocalImportNormalizers,$1)),
				$(call $0,$(invoke 1->eContainer))),
			$(and $1,$(call $0,$(invoke 1->eContainer))))
	)

	# Param:
	#   1. Scope container.
	# Return:
	#   Implementation have to return named normalizers supposed to resolve
	#   members of the given container.
	#   By default returns 'fully.qualified.name.*'
	$(method getLocalElementNormalizers,
		$(for name <- $(get 1->qualifiedName),
			$(name).)*)

	# Param:
	#   1. Scope container.
	# Return:
	#   Implementation have to return list of import normalization patterns
	#   local to the given object. By default looks for an attribute named
	#   'imports' and returns its value (if any).
	$(method getLocalImportNormalizers,
		$(for metaAttribute <- $(get $(get 1->eMetaClass).eAllAttributes),
			$(if $(eq imports,$(get metaAttribute->name)),
				$(get 1->$(get metaAttribute->instanceProperty)))))

	# Context:
	#   'resource'. Resource containing the link.
	#   'linkName'. Name to find.
	#   'linkPrefix'. The firs segment of the link name.
	#   'targetType'. Meta class of the object being resolved.
	# Return:
	#   List of named elements of type 'targetType',
	#   who's normalized name equals to 'linkName'.
	$(method searchGlobalScopeUsingResourceImports,
		$(call Linker.searchGlobalScopeWithNormalizers,
			$(invoke getResourceImportNormalizers,$(resource))))

	# Param:
	#   1. The resource.
	# Return:
	#   Implementation have to return list of implicit imports of the given
	#   resource. By default returns nothing.
	$(method getResourceImportNormalizers,
		)

	# Param:
	#   1. List of normalizers.
	# Context:
	#   'linkName'. Name to find.
	#   'linkPrefix'. The first segment of the link name.
	#   'targetType'. Meta class of the object being resolved.
	# Return:
	#   List of named elements of type 'targetType',
	#   who's name relative to the given container equals to 'linkName'.
	$(method searchGlobalScopeWithNormalizers,# static
		$(if $(strip $1),$(with \
			# 1. List of matching objects with proper type.
			$(for normalizedName <- $(call Linker.normalizeLinkName,$1),
				matchingObject <- $(map-get sourceSet->exportedObjectsMap
						/$(normalizedName)),

				# Check the type of the object.
				$(if $(invoke targetType->isInstance,$(matchingObject)),
					$(matchingObject))
			),

#			$(silent-for 1 <- $1,$(warning >>> $1))
			$(or $(notdir $(singleword $1)),
				$(if $(strip $1),
					$(warning Multiple import matches: $1)))
		))
	)

	# Tries to normalize the 'linkName' using given normalizers.
	# If possible, non-wildcard normalizers are used, and if the result is not
	# empty, it is returned without even trying to match wildcard normalizers.
	# Otherwise substitutes every wildcard with the 'linkName' and returns it.
	#
	# Param:
	#   1. List of normalizers.
	# Context:
	#   'linkName'. Name to find.
	#   'linkPrefix'. The first segment of the link name.
	# Return:
	#   List of normalized names.
	$(method normalizeLinkName,# static

		# Get list of matching normalizers and replace the last segment
		# with the full link name.
		$(addsuffix .$(linkName),

			# Cut the last segment, which is either '.linkPrefix' or '.*'.
			$(basename \
				$(or \
					# Search for an exact match (no wildcards).
					$(filter %.$(linkPrefix),$1),

					# Get all wildcard normalizers.
					$(filter %.*,$1))))
	)

	# Context:
	#   'linkName'. Name to find.
	#   'targetType'. Meta class of the object being resolved.
	# Return:
	#   List of named elements of type 'targetType',
	#   who's name equals to 'linkName', with no normalization applied.
	$(method searchGlobalScopeByFullName,
		$(suffix \
			$(for matchingObject <-
				$(map-get sourceSet->exportedObjectsMap/$(linkName)),

				$(if $(invoke targetType->isInstance,$(matchingObject)),
					$(matchingObject)))
		)
	)

endef

$(def_all)

endif # __model_linkage_mk

#
#   Date: Jan 22, 2012
# Author: Eldar Abusalimov
#

ifndef __model_linkage_mk
__model_linkage_mk := 1

include mk/model/model.mk
include mk/model/metamodel.mk

define class-LinkageUnit

	# List of exported named objects.
	$(property exportedObjects... : ENamedObject)

	# List of not yet resolved links.
	$(property unresolvedLinks... : ELink)

	$(field __exportedObjectsWithNamePrefixes... : ELink)

	$(method link,
		$(with $(get-field __exportedObjectsWithNamePrefixes),
			# Construct and push the list of currently exported objects.
			$(set-field __exportedObjectsWithNamePrefixes,
				$(for o <- $(get exportedObjects),$(get o->qualifiedName)/$o))

			$(invoke __link)

			# Pop.
			$(set-field __exportedObjectsWithNamePrefixes,$1)
		)
	)

	$(method __link,
		$(for link     <- $(get unresolvedLinks),
			linkName   <- $(get link->name),
			linkNamePrefix <- $(firstword $(subst ., ,$(linkName))),
			source     <- $(get link->eSource),
			reference  <- $(get link->eMetaReference),
			targetType <- $(get reference->eReferenceType),
			$(with \
				$(or \
					$(invoke lookupContainerChain,
						$(source),searchLocalScopeOf),
					$(invoke lookupContainerChain,
						$(source),searchGlobalScopeUsingImportNormalizersOf),
#					$(invoke searchGlobalScopeByFullName),
					$(warning can't resolve $(linkName) \
						[reference=$(get reference->name)])),
				$(warning >>> '$(linkName)' -> '$1')
				$(if $(singleword $1),
					$(set link->eTarget,$1))
			)
		)
	)

	# Param:
	#   1. Link source.
	#   2. Method to invoke on each container until it returns non-empty value.
	$(method lookupContainerChain,
		$(with $(get 1->eContainer),$2,
			$(if $1,
				$(or $(invoke $2,$1),
					$(call $0,$(get 1->eContainer),$2))))
	)

	# Param:
	#   1. Scope container.
	# Context:
	#   'link'. The link being resolved.
	#   'linkName'. Name to find.
	#   'targetType'. Meta class of the target object.
	# Return:
	#   List of named elements of type 'targetType',
	#   who's name relative to the given container equals to 'linkName'.
	$(method searchLocalScopeOf,
		$(strip $(with $1,.,
			# 1. Object to scan for members.
			# 2. Prefix accumulating the member names delimited by a period.
			#    Also starts and ends with periods: '.prefix.'
			$(for namedChild <- $(invoke getVisibleMembers,$1),
				childName <- $(get namedChild->name),

				$(if $(filter $2$(childName).,.$(linkName).),
					$(if $(invoke targetType->isInstance,$(namedChild)),
						$(namedChild)),
					$(if $(filter $2$(childName).%,.$(linkName).),
						$(call $0,$(namedChild),$2$(childName).))
				)
			)
		))
	)

	# Param:
	#   1. Scope container.
	# Return:
	#   Implementation have to return named objects supposed to be a members
	#   of the given container. By default returns named descendants, either
	#   direct or indirect being first found in each brach of the containment
	#   subtree.
	$(method getVisibleMembers,
		$(for metaReference <- $(get $(get 1->eMetaClass).eAllContainments),
			$(if $(invoke EModel_ENamedObject->isSuperTypeOf,
					$(get metaReference->eReferenceType)),
				$(get 1->$(get metaReference->instanceProperty)),
				$(for child <-
					$(get 1->$(get metaReference->instanceProperty)),
					$(call $0,$(child)))
			)
		)
	)

	# Param:
	#   1. Scope container.
	# Context:
	#   'linkName'. Name to find.
	#   'linkNamePrefix'. The firs segment of the link name.
	#   'targetType'. Meta class of the object being resolved.
	# Return:
	#   List of named elements of type 'targetType',
	#   who's name relative to the given container equals to 'linkName'.
	$(method searchGlobalScopeUsingImportNormalizersOf,
		$(with $(strip $(invoke getImportNormalizers,$1)),$(if $1,$(with \
			$(for matchingObject <-
				$(filter \
					# Get list of matching imports, replace the last segment
					# with the full link name, and append a percent.
					$(addsuffix .$(linkName)/%,$(basename $(or \
						# Search for an exact match (no wildcards).
						$(filter %.$(linkNamePrefix),$1),
						# Replace wildcards with our name.
						$(filter $1,
							$(addsuffix .$(linkNamePrefix),$(basename $1)))))),
					# List of 'f.q.n/object' items.
					$(get-field __exportedObjectsWithNamePrefixes)),

				# Check the type of the object.
				$(if $(invoke targetType->isInstance,$(matchingObject)),
					$(matchingObject))
			),

#			$(silent-for 1 <- $1,$(warning >>> $1))
			$(or $(notdir $(singleword $1)),
				$(if $(strip $1),
					$(warning Multiple import matches: $1)))
		)))
	)

	# Param:
	#   1. Scope container.
	# Return:
	#   Implementation have to return list of import normalization patterns
	#   local to the given object. By default looks for an attribute named
	#   'imports' and returns its value (if any).
	$(method getImportNormalizers,
		$(for metaAttribute <- $(get $(get 1->eMetaClass).eAllAttributes),
			$(if $(eq imports,$(get metaAttribute->name)),
				$(get 1->$(get metaAttribute->instanceProperty)))
		)
	)

	$(method unlink)

endef

define class-CompositeLinkageUnit
	$(super LinkageUnit)

	$(property children... : LinkageUnit)

	$(getter exportedObjects,
		$(for child <- $(get children),
			$(get child->exportedObjects)))

	$(getter unresolvedLinks,
		$(for child <- $(get children),
			$(get child->unresolvedLinks)))

endef


$(def_all)

endif # __model_linkage_mk

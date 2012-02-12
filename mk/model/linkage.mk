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

	$(method link,
		$(with $(for o <- $(get exportedObjects),$(get o->qualifiedName)/$o),
			$(for link <- $(get unresolvedLinks),
				linkName <- $(get link->name),
				reference <- $(get link->eMetaReference),
				targetType <- $(get reference->eReferenceType),
				$(or \
					$(with $(get $(get link->eSource).eContainer),
						$(if $1,$(or $(invoke searchLocalScopeOf,$1),
									$(call $0,$(get 1->eContainer))))),
					$(notdir $(filter $(linkName)/%,$1)),
					$(warning can't resolve $(linkName) [reference=$(get reference->name)])
				)
			)
		)
	)

	# Param:
	#   1. Scope container.
	# Context:
	#   'linkName'. Name to find.
	#   'targetType'. Meta class of the object being resolved.
	$(method searchLocalScopeOf,
		$(strip $(with $1,,
			$(for namedChild <- $(invoke getVisibleMembersOf,$1),
				childName <- $(get namedChild->name),

				$(if $(filter $2.$(childName).,.$(linkName).),
					$(if $(invoke targetType->isInstance,$(namedChild)),
						$(namedChild)),
					$(if $(filter $2.$(childName).%,.$(linkName).),
						$(call $0,$(namedChild),$2.$(childName)))
				)
			)
		))
	)

	# Param:
	#   1. Scope container.
	# Return:
	#   Implemantation have to return named objects supposed to be a members
	#   of the given container. By default returns named descendants, either
	#   direct or indirect being first found in each brach of the subtree.
	$(method getVisibleMembersOf,
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

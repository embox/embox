#
# Builtins for generated implementations of modeled objects
# and runtime support for managing object references.
#
#   Date: Feb 17, 2012
# Author: Eldar Abusalimov
#

ifndef __model_eobject_mk
__model_eobject_mk := 1

# Param:
#   1. Meta class ID.
#   2. Instance class name.
#   3. Super classes.
#   4. Flags, which may contain:
#        abstract, interface
define builtin_func-eobject
	$(call __def,

		# Implements API class.
		$$(super $2)

		# Extends implementations of each super class.
		$(foreach s,$(or $3,EObject),
			$$(super $sImpl))

		$$(getter eMetaClassId,
			$1)
	)
endef

# Param:
#   1. Meta feature ID.
#   2. Instance property name.
#   3. Flags, which may contain:
#        changeable many
define builtin_func-eobject-attribute
	$(call __def,
		$(if $(filter changeable,$3),
			# Changeable attributes are just property-fields.
			$$(property-field $2$(if $(filter many,$3),...)),

			# Otherwise, only getter should be defined.
			$$(property $2$(if $(filter many,$3),...))
			$$(field $2$(if $(filter many,$3),...))
			$$(getter $2,
				$$(get-field $2))
		)
	)
endef

# Param:
#   1. Meta feature ID.
#   2. Instance property name.
#   3. Reference class.
#   4. Opposite property name (if any).
#   5. Flags, which may contain:
#        changeable many containment container linkable
define builtin_func-eobject-reference
	$(call __def,

		# Declare a propery (though it has already been declared in API class).
		$$(property $2$(if $(filter many,$5),...) : $3)

		# Generate getter.
		$(if $(filter container,$5),
			# Container references share the same field.
			$$(getter $2,
				$$(suffix $$(filter $2/%,$$(get-field __eContainer)))),

			# Otherwise define a new field.
			$(if $(filter containment,$5),
				$$(field $2$(if $(filter many,$5),...) : $3),

				# Cross references are stored in raw fields.
				$$(field $2$(if $(filter many,$5),...))
				# There is also a custom serializer for such fields.
				$$(method __serialize_field-$2,
					$$(call __eObjectSerializeCrossReference,$2,$1))
			)
			$$(getter $2,
				# Getting suffix is mandatory here!
				$$(suffix $$(get-field $2)))
		)

		$(for fn_suffix <-
			$(if $(filter containment,$5),Containment,
				$(if $(filter container,$5),Container,
					$(if $4,Bidirectional,Unidirectional))),
			property <- $2,

			$(__eobject_ref_setters)

			$(if $(filter linkable,$5),
				$(for property <- $(property)_link$(if $(filter many,$5),s),
					fn_suffix <- $(fn_suffix)_link,

					$$(getter $(property),
						$$(subst ./,,$$(dir $$(get-field $2))))

					$(__eobject_ref_setters)
				)
			)
		)
	)
endef

# Param:
#   Same as to 'eobject-reference' builtin.
# Context:
#   'property'
#   'fn_suffix'
define __eobject_ref_setters
	$(if $(filter changeable,$5),
		$$(setter $(property),
			$$(call __eObjectSet$(fn_suffix),$(__eobject_ref_setter_args)))
		$(if $(filter many,$5),
			$$(setter+ $(property),
				$$(call __eObjectAdd$(fn_suffix),$(__eobject_ref_setter_args)))
			$$(setter- $(property),
				$$(call __eObjectRemove$(fn_suffix),$(__eobject_ref_setter_args)))
		)
	)
endef

# Param:
#   Same as to 'eobject-reference' builtin.
__eobject_ref_setter_args = \
	$2,$$(suffix $$1),$4,$1

# Params:
#   1. Property name.
#   2. Meta reference ID.
define __eObjectSerializeCrossReference
	$(assert $(not $(or $(get $2->isContainment),$(get $2->isContainer))),
		$0: Can't handle non-cross references)
	# '.link./.target'
	# '.link./'
	#        '.target'
	#   '.link.target'
	$(for r <- $(get-field $1),
		$(for l <- $(subst ./,,$(dir $r)),# Link.
			$($l.__serial_id__)./)
		$(for t <- $(suffix $r),# Target.
			$(for l <- $(basename $(notdir $r)),# Opposite link.
				$($l.__serial_id__))
			$($t.__serial_id__)))
endef

# Params:
#   1. Property name.
define __eObjectGetContainer
	$(filter $1/%,$(get-field __eContainer))
endef

# Params:
#   1. Property name.
#   2. New container.
#   3. Containment property in the container.
define __eObjectSetContainer
	$(assert $(not $(multiword $2)))

	$(for oldContainer <- $(get-field __eContainer),
		$(set-field- oldContainer->$(notdir $(basename $(oldContainer))),
			$(this))
	)

	$(set-field __eContainer,$1/$3$2)

	$(for newContainer <- $2,
		$(set-field+ newContainer->$3,$(this))
	)
endef

# Params:
#   1. Property name.
#   2. What to add.
#   3. Opposite property.
define __eObjectAddContainment
	$(silent-for this <- $2,
		# XXX Using '__this'. -- Eldar
		$(call __eObjectSetContainer,$3,$(__this),$1))
endef

# Params:
#   1. Property name.
#   2. What to add.
define __eObjectAddUnidirectional
	$(set-field+ $1,$2)
	$(silent-for e <- $2,
		$(set-field+ e->__eOppositeRefs,$1$(this)))
endef

# Params:
#   1. Property name.
#   2. What to add.
#   3. Opposite property.
define __eObjectAddBidirectional
	$(set-field+ $1,$2)
	$(silent-for e <- $2,
		$(set-field+ e->$3,$(this)))
endef

# Params:
#   1. Property name.
#   2. What to remove.
#   3. Opposite property.
define __eObjectRemoveContainment
	$(foreach ,$2,$(warning $0: NIY))
endef

# Params:
#   1. Property name.
#   2. What to remove.
define __eObjectRemoveUnidirectional
	$(foreach ,$2,$(warning $0: NIY))
endef

# Params:
#   1. Property name.
#   2. What to remove.
#   3. Opposite property.
define __eObjectRemoveBidirectional
	$(foreach ,$2,$(warning $0: NIY))
endef

# Params:
#   1. Property name.
#   2. New value.
#   3. Opposite property.
define __eObjectSetContainment
	$(call __eObjectRemoveContainment,$1,$(get-field $1),$3)
	$(call __eObjectAddContainment,$1,$2,$3)
endef

# Params:
#   1. Property name.
#   2. New value.
define __eObjectSetUnidirectional
	$(call __eObjectRemoveUnidirectional,$1,$(get-field $1))
	$(call __eObjectAddUnidirectional,$1,$2)
endef

# Params:
#   1. Property name.
#   2. New value.
#   3. Opposite property.
define __eObjectSetBidirectional
	$(call __eObjectRemoveBidirectional,$1,$(get-field $1),$3)
	$(call __eObjectAddBidirectional,$1,$2,$3)
endef

# Params:
#   1. Property name.
#   2. What to add.
#   3. Empty.
#   4. Meta reference ID.
define __eObjectAddUnidirectional_link
	$(set-field+ $1,
		$(for link <- $2,
			$(set-field link->eReferenceSource,$4$(this))
			# 'link./target' for resolved links, 'link./' otherwise.
			$(link)./$(for target <- $(get link->eTarget),
						$(set-field+ target->__eOppositeRefs,$(link)/$1$(this))
						$(target)))
	)
endef

# Params:
#   1. Property name.
#   2. What to add.
#   3. Opposite property.
#   4. Meta reference ID.
define __eObjectAddBidirectional_link
	$(set-field+ $1,
		$(for link <- $2,
			$(set-field link->eReferenceSource,$4$(this))
			# 'link./target' for resolved links, 'link./' otherwise.
			$(link)./$(for target <- $(get link->eTarget),
						$(set-field+ target->$3,$(link)$(this))
						$(target)))
	)
endef

# Params:
#   1. Property name.
#   2. What to remove.
define __eObjectRemoveUnidirectional_link
	$(foreach ,$2,$(warning $0: NIY))
endef

# Params:
#   1. Property name.
#   2. What to remove.
#   3. Opposite property.
define __eObjectRemoveBidirectional_link
	$(foreach ,$2,$(warning $0: NIY))
endef

# Params:
#   1. Property name.
#   2. New value.
#   3. Empty.
#   4. Meta reference ID.
define __eObjectSetUnidirectional_link
	$(call __eObjectRemoveUnidirectional_link,$1,$(get-field $1))
	$(call __eObjectAddUnidirectional_link,$1,$2,,$4)
endef

# Params:
#   1. Property name.
#   2. New value.
#   3. Opposite property.
#   4. Meta reference ID.
define __eObjectSetBidirectional_link
	$(call __eObjectRemoveBidirectional_link,$1,$(get-field $1),$3)
	$(call __eObjectAddBidirectional_link,$1,$2,$3,$4)
endef

# Params:
#   1. New value of 'eTarget' property of this link.
define __eLinkSetTarget
	$(assert $(not $(multiword $1)))
	$(assert $(get eSource),
		Can't set a target on the link with no source)

	$(for oldTarget <- $(get-field eTarget),
		$(warning $0: NIY))

	$(for newTarget <- $1,
		source <- $(get eSource),
		metaReference <- $(get eMetaReference),
		referenceProperty <- $(get metaReference->instanceProperty),

		$(set-field eTarget,$(newTarget))

		$(set-field source->$(referenceProperty),
			$(patsubst $(this)./,$(this)./$(newTarget),
				$(get-field source->$(referenceProperty))))

		$(if $(for opposite <- $(get metaReference->eOpposite),
				oppositeProperty <- $(get opposite->instanceProperty),
				$(set-field+ newTarget->$(oppositeProperty),$(this)$(source))
				$(opposite)),,# else
			$(set-field+ newTarget->__eOppositeRefs,
				$(this)/$(referenceProperty)$(source)))
	)
endef

$(def_all)

endif # __model_eobject_mk


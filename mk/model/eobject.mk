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
			$$(field $2$(if $(filter many,$5),...) : $3)
			$$(getter $2,
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
						$$(call __eObjectGet$(fn_suffix),$2))

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
define __eObjectAddUnidirectional
	$(set-field+ $1,$2)
	$(silent-for e <- $2,
		$(set-field+ e->__eOppositeRefs,$1$(this)))
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
#   3. Opposite property.
define __eObjectAddBidirectional
	$(set-field+ $1,$2)
	$(silent-for e <- $2,
		$(set-field+ e->$3,$(this)))
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
#   2. What to remove.
#   3. Opposite property.
define __eObjectRemoveContainment
	$(foreach ,$2,$(warning $0: NIY))
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
#   2. New value.
#   3. Opposite property.
define __eObjectSetContainment
	$(call __eObjectRemoveContainment,$1,$(get-field $1),$3)
	$(call __eObjectAddContainment,$1,$2,$3)
endef

# Params:
#   1. Property name.
#   2. What to add.
#   3. Empty.
#   4. Meta reference ID.
define __eObjectAddUnidirectional_link
	$(with $1,$2,
		# Resolved links suffixed by destination.
		$(for link <- $2,
			$(set-field link->eMetaReferenceId,$4)
			$(set-field link->eSource,$1/eLinks$(this))
			$(for dst <- $(get link->eTarget),
				$(link)$(dst))),

		$(set-field+ $1,$3)
		$(silent-for link_dst <- $3,
			$(set-field+ link_dst->__eOppositeRefs,
				$1$(basename $(link_dst))$(this)))

		$(set-field+ __eUnresolvedLinks,
			$(addprefix $1/,$(filter-out $(basename $3),$2)))
	)
endef

# Params:
#   1. Property name.
#   2. What to add.
#   3. Opposite property.
#   4. Meta reference ID.
define __eObjectAddBidirectional_link
	$(with $1,$2,$3,
		# Resolved links suffixed by destination.
		$(for link <- $2,
			$(set-field link->eMetaReferenceId,$4)
			$(set-field link->eSource,$1/eLinks$(this))
			$(for dst <- $(get link->eTarget),
				$(link)$(dst))),

		$(set-field+ $1,$4)
		$(silent-for link_dst <- $4,
			$(set-field+ link_dst->$3,$(basename $(link_dst))$(this)))

		$(set-field+ __eUnresolvedLinks,
			$(addprefix $1/$3,$(filter-out $(basename $4),$2)))
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


$(def_all)

endif # __model_eobject_mk


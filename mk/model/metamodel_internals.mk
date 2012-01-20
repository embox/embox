#
# Metamodel internal utils.
#
#   Date: Jan 20, 2011
# Author: Eldar Abusalimov
#

ifndef __model_metamodel_internals_mk
__model_metamodel_internals_mk := 1

# TODO Proper prefixes on functions. -- Eldar

#
# Meta object instantiation.
#

# Params:
#   1. Meta model ID (unused).
define createMetaModel
	$(for model <- $(invoke eModelFactory->createEMetaModel),
		# Do nothing special.
		$(model))
endef

# Params:
#   1. Meta model.
#   2. Meta class ID (unused).
define createMetaClass
	$(for class <- $(invoke eModelFactory->createEMetaClass),
		$(set+ 1->eTypes,$(class))
		$(class))
endef

# Params:
#   1. Meta class.
#   2. Meta feature ID (unused).
define createMetaAttribute
	$(for feature <- $(invoke eModelFactory->createEMetaAttribute),
		$(set+ 1->eFeatures,$(feature))
		$(feature))
endef

# Params:
#   1. Meta class.
#   2. Meta feature ID (unused).
define createMetaReference
	$(for feature <- $(invoke eModelFactory->createEMetaReference),
		$(set+ 1->eFeatures,$(feature))
		$(feature))
endef

#
# Objects initialization.
#

# Params:
#   1. Meta model.
#   2. Name.
define initMetaModel
	$(set 1->name,$2)
endef

# Params:
#   1. Meta class.
#   2. Name.
#   3. Super types...
#   4. Flags...
define initMetaClass
	$(set 1->name,$2)
	$(set 1->eSuperTypes,$3)
	$(set 1->isAbstract,$(filter abstract,$4))
	$(set 1->isInterface,$(filter interface,$4))
endef

# Params:
#   1. Meta attribute.
#   2. Name.
#   3. Lower bound.
#   4. Upper bound.
#   5. Flags...
define initMetaAttribute
	$(call commonInitMetaFeature,$1,$2,$3,$4,$5)
endef

# Params:
#   1. Meta reference.
#   2. Name.
#   3. Lower bound.
#   4. Upper bound.
#   5. Referenced class.
#   6. Opposite reference (if any).
#   7. Flags...
define initMetaReference
	$(call commonInitMetaFeature,$1,$2,$3,$4,$7)
	$(set 1->eType,$5)
	$(set 1->eOpposite,$6)
	$(set 1->isContainment,$(filter containment,$7))
#		$(set 1->isContainer,$(filter container,$7))
endef

# Params:
#   1. Meta feature.
#   2. Name.
#   3. Lower bound.
#   4. Upper bound.
#   5. Flags...
define commonInitMetaFeature
	$(set 1->name,$2)
	$(set 1->lowerBound,$3)
	$(set 1->upperBound,$4)
	$(set 1->isChangeable,$(filter changeable,$4))
	$(set 1->isDerived,$(filter derived,$4))
	$(set 1->isVolatile,$(filter volatile,$4))
	$(set 1->isTransient,$(filter transient,$4))
endef

#
# Binding to native class/properties.
#

# Params:
#   1. Meta class.
#   2. Instance class name.
define bindMetaClass
	$(assert $(class-exists $2),
		Can't bind meta type '$(get 1->name)' to undefined class '$2')
	$(set 1->instanceClass,$2)
endef

# Params:
#   1. Meta feature.
#   2. Instance property name.
define bindMetaFeature
	$(assert $(class-has-property \
			$(get $(get 1->eContainingClass).instanceClass),$2),
		Can't bind meta feature '$(get 1->name)' to undefined property '$2'
		of class '$(get $(get 1->eContainingClass).instanceClass)')
	$(set 1->instanceProperty,$2)
endef

$(def_all)

endif # __model_metamodel_internals_mk


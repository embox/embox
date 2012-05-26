#
#   Date: May 15, 2012
# Author: Anton Kozlov
#

ifndef __mybuild_annotations_core_mk
__mybuild_annotations_core_mk := 1

include mk/core/define.mk

define class-AnnotationsCore
	$(map avaibleAnnotations : AnnotationCallback)

	$(method addSupported,
		$(map-set avaibleAnnotations/$1/$2,$(new $3)))

	$(method getSupported,
		$(map-get avaibleAnnotations/$1/$2))

	$(for fileMain <- $(value 1),
		$(if $(value $(fileMain)),
			$(call $(fileMain),$(this))))

	$(method new,
		$(for obj <- $1,
			$(set obj->target,$2)
			$(set obj->annotationType,$(get 3->type))

			$(for \
				annotation <- $3,
				annotationBinding <- $(get annotation->bindings),
				option <- $(get annotationBinding->option),
				optionName <- $(get option->name),
				optionValue <- $(get annotationBinding->value),

				$(map-set obj->options/$(optionName),$(get optionValue->value)))
			$(obj)))

endef

define class-AnnotationCallback
	$(map options)
	$(property-field target)
	$(property-field annotationType)

	$(method callback,
		$(invoke $1Callback,$(value 2)))

	$(method MyLinkCallback)
	$(method BuildCallback)

endef

# Args:
#	1. Build phase
#	2. Target
#	3. Optional
define mybuild_annotation_process
	$(with \
			$1,
			$2,
			$(for \
				target <- $2,
				annotation <- $(get target->annotations),
				annotationType <- $(get annotation->type),

				callbackClass <- $(invoke annotationsCore->getSupported,$1,
					$(get annotationType->qualifiedName)),
				$(invoke annotationsCore->new,$(callbackClass),$(target),
					$(annotation))),
			$(value 3),


		$(if $(strip $3),
			$(invoke 3->callback,$1,$4),
			$2))

endef

$(def_all)

annotation_handler_init = $(lastword $(MAKEFILE_LIST))

endif # __mybuild_annotations_core_mk

#
#   Date: May 15, 2012
# Author: Anton Kozlov
#

ifndef __mybuild_annotations_core_mk
__mybuild_annotations_core_mk := 1

include mk/core/define.mk

define class-AnnotationsCore
	$(map avaibleAnnotations : AnnotationCallbackFactory)

	$(method addSupported,
		$(map-set avaibleAnnotations/$1/$2,$3))

	$(method getSupported,
		$(map-get avaibleAnnotations/$1/$2))

	$(for fileMain <- $(value 1),
		$(call $(fileMain),$(this)))
endef

define class-AnnotationCallback
	$(map options)
	$(property-field target)
	$(property-field annotationType)

	$(method callback,
		$(invoke $1Callback))

	$(method MyFileCallback)


	$(method BuildCallback)

endef

define class-AnnotationCallbackFactory
	$(property-field annotationCallbackClassName,$1)

	$(method new,
		$(for obj <- $(new $(get annotationCallbackClassName)),
			$(set obj->target,$1)
			$(set obj->annotationType,$(get 2->type))

			$(for \
				annotation <- $2,
				annotationBinding <- $(get annotation->bindings),
				option <- $(get annotationBinding->option),
				optionName <- $(get option->name),
				optionValue <- $(get annotationBinding->value),

				$(map-set obj->options/$(optionName),$(get optionValue->value)))
			$(obj)))

endef

define myfile_annotation_callbacks
	$(for \
		target <- $1,
		annotation <- $(get target->annotations),
		annotationType <- $(get annotation->type),
		callbackClass <- $(invoke annotationsCore->getSupported,$2,
			$(get annotationType->qualifiedName)),
		callbackObj <- $(invoke callbackClass->new,$(target),$(annotation)),

		$(invoke callbackObj->callback,$2))
endef

$(def_all)

annotation_handler_init = $(lastword $(MAKEFILE_LIST))

endif # __mybuild_annotations_core_mk

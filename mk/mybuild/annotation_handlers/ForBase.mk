#
#   Date: May 15, 2012
# Author: Anton Kozlov
#

ifndef __mk_annotations_handler_for_base_mk
__mk_annotations_handler_for_base_mk := 1

define class-ForBaseAnnotationCallback
	$(super AnnotationCallback)

	$(method checkAnnotationForParent,
		$(assert $(invoke ConfigFile_Include->isInstance,$(get target)))
		$(assert $(map-get options/value))
		$(for \
			parent <- $(map-get options/value),
			include <- $(get target),
			mybuild <- $1,

			$(if $(invoke parent->isSuperTypeOf,$(get include->module)),
				$(include),
				$(invoke mybuild->addIssueInclude,$(include),error,
					 Annotation $(get $(get annotationType).qualifiedName) value should be \
						 target's parent))))

endef

$(def_all)

endif # __mk_annotations_handler_for_base_mk

#
#   Date: May 15, 2012
# Author: Anton Kozlov
#

ifndef __mk_annotations_handler_default_impl_mk
__mk_annotations_handler_default_impl_mk := 1

define $(annotation_handler_init)
	$(for \
		factory <- $(new AnnotationCallbackFactory,DefaultImplAnnotationCallback),
		context <- Build MyLink,
		$(invoke 1->addSupported,$(context),mybuild.lang.DefaultImpl,$(factory)))
endef

define class-DefaultImplAnnotationCallback
	$(super AnnotationCallback)

	$(method MyLinkCallback,
		$(assert $(invoke MyFile_ModuleType->isInstance,$(get target)))
		$(assert $(get target>isAbstract))
		$(assert $(map-get options/value))
		$(for \
			abstract <- $(get target),
			defaultImpl <- $(map-get options/value),
			$(if $(invoke defaultImpl->isSubTypeOf,$(abstract)),
				$(abstract),
				$(invoke 1->addIssues,$(new BaseIssue,
					$1,
					error,
					$(get target>origin),
					DefaultImpl annotation value should be subtype of target and)))))

	$(method BuildCallback,
		$(warning $0: )
		$(for \
			abstract <- $(get target),
			defaultImpl <- $(map-get options/value),
			$(if 1->getRecommendation,$(abstract),,
				$(invoke 1->addRecommendationRule,$(abstract),$(defaultImpl)))
			$(abstract)))


endef

$(def_all)

endif # __mk_annotations_handler_default_impl_mk

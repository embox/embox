#
#   Date: May 17, 2012
# Author: Anton Kozlov
#

ifndef __mk_annotations_handler_test_for_mk
__mk_annotations_handler_test_for_mk := 1

define $(lastword $(MAKEFILE_LIST))
	$(for context <- MyLink Build,
		$(invoke 1->addSupported,$(context),mybuild.lang.TestFor,
			$(new AnnotationCallbackFactory,TestForAnnotationCallback)))

	$(invoke 1->addSupported,Build,mybuild.lang.WithTest,
		$(new AnnotationCallbackFactory,WithTestAnnotationCallback))

endef

define class-TestForAnnotationCallback
	$(super AnnotationCallback)

	$(method MyLinkCallback,
		$(map-set+ $(map-get options/value).customStorage/tests,$(get target)))

	$(method BuildCallback,
		$(for \
			targetInstance <- $(invoke 1->includeModule,$(map-get options/value)),
			thisInstance <- $(invoke 1->moduleInstance,$(get target)),
			$(set* targetInstance->afterDepends,$(thisInstance)))
		$(get target))

endef

define class-WithTestAnnotationCallback
	$(super AnnotationCallback)

	$(method BuildCallback,
		$(for \
			include <- $(get target),
			test <- $(map-get options/value),
			mod <- $(get target>module),
			$(if $(filter $(test),$(map-get mod->customStorage/tests)),
				$(set+ include->module,$(test)),
				$(invoke 1->addIssueInclude,$(get target),error,$(get test->qualifiedName) \
					is not $(get mod->qualifiedName) test)))
		$(get target))
endef

define class-WithAllTestsAnnotationCallback
	$(super AnnotationCallback)

	$(method BuildCallback,
		$(for \
			include <- $(get target),
			mod <- $(get target>module),
			test <- $(map-get mod->customStorage/tests),

			$(set+ include->module,$(test)))
		$(get target))
endef

$(def_all)

endif # __mk_annotations_handler_test_for_mk

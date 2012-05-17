#
#   Date: May 15, 2012
# Author: Anton Kozlov
#

ifndef __mk_annotations_handler_ifneed_mk
__mk_annotations_handler_ifneed_mk := 1

define $(lastword $(MAKEFILE_LIST))
	$(invoke 1->addSupported,Build,mybuild.lang.IfNeed,
		$(new AnnotationCallbackFactory,IfNeedAnnotationCallback))
endef

include $(ANNOTATION_HANDLERS)/ForBase.mk

define class-IfNeedAnnotationCallback
	$(super ForBaseAnnotationCallback)

	$(method BuildCallback,
		$(if $(invoke checkAnnotationForParent,$1),
			$(invoke 1->addRecommendationRule,$(map-get options/value),$(get target>module))))


endef

$(def_all)

endif # __mk_annotations_handler_ifneed_mk

#
#   Date: May 15, 2012
# Author: Anton Kozlov
#

ifndef __mk_annotations_handler_for_mk
__mk_annotations_handler_for_mk := 1

define $(annotation_handler_init)
	$(invoke 1->addSupported,Build,mybuild.lang.For,
		$(new AnnotationCallbackFactory,ForAnnotationCallback))
endef

include $(ANNOTATION_HANDLERS)/ForBase.mk

define class-ForAnnotationCallback
	$(super ForBaseAnnotationCallback)

	$(method BuildCallback,
		$(invoke checkAnnotationForParent,$1))

endef

$(def_all)

endif # __mk_annotations_handler_for_mk

#
#   Date: May 15, 2012
# Author: Anton Kozlov
#

ifndef __mk_mybuild_annotations_handler_test_mk
__mk_mybuild_annotations_handler_test_mk := 1

define class-PrintAnnotationCallback
	$(super AnnotationCallback)

	$(method MyFileCallback,
		$(assert $(invoke MyFile_SourceMember->isInstance,$(target)))
		$(assert $(map-get options/value)))

		#$(warning \
			#$(for \
				#sourceMember <- $(target),
				#file <- $(get sourceMember->files),
				#$(get file->fileFullName) has $(map-get options/value) preprocessor option)))

endef

define $(annotation_handler_init)
	$(invoke 1->addSupported,MyFile,mybuild.lang.IncludePath,
		$(new AnnotationCallbackFactory,PrintAnnotationCallback))
endef

$(def_all)

endif # __mk_mybuild_annotations_handler_test_mk

#
#   Date: Mar 5, 2012
# Author: Eldar Abusalimov
# Author: Anton Kozlov
#

ifndef __mybuild_myfile_linkage_mk
__mybuild_myfile_linkage_mk := 1

include mk/model/linkage.mk

define class-MyFileLinker
	$(super Linker,$1,$2)

	$(method getResourceImportNormalizers,
		$(for root <- $(get 1->contentsRoot),
			$(with $(get root->name),
				$(if $1,$1.* )))
		mybuild.lang.*)

	$(method linkHandle-MyFile_OptionBinding_option,
		$(for \
			optBind <- $(invoke 1->eSource),
			annot <- $(invoke optBind->eContainer),

		#	$(for link <- $(invoke annot->eUnresolvedLinks),
		#		$(call Linker.doSingleLink,$(link)))

			$(for annotType <- $(get annot->type),
				opt <- $(get annotType->options),

				$(if $(eq $(get opt->name),$(get link->name)),
					$(opt)))))


endef

endif # __mybuild_myfile_linkage_mk

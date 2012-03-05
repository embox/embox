#
#   Date: Mar 5, 2012
# Author: Eldar Abusalimov
# Author: Anton Kozlov
#

ifndef __mybuild_myfile_linkage_mk
__mybuild_myfile_linkage_mk := 1

include mk/model/linkage.mk

define class-MyFileLinker
	$(super Linker)

	$(method getResourceImportNormalizers,
		$(for root <- $(get 1->contentsRoot),
			$(with $(get root->name),
				$(if $1,$1.*))))

endef

endif # __mybuild_myfile_linkage_mk

#
#   Date: Jan 21, 2012
# Author: Eldar Abusalimov
#

ifndef __mybuild_myfile_resource_mk
__mybuild_myfile_resource_mk := 1

include mk/model/resource.mk
include mk/mybuild/myfile-parser.mk

define class-MyFileResource
	$(super Resource,$(value 1))

	# Param:
	#   1. File name.
	# Return:
	#   Result of parsing or empry on error.
	$(method loadRootObject,
		$(for resource <- $(this),
			$(call gold_parse,myfile,$1,
				$(lambda $(set+ resource->issues,
					$(new Issue,$(resource),$2,$3,$4)))))
	)

endef

$(def_all)

endif # __mybuild_myfile_resource_mk

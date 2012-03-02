#
#   Date: Mar 2, 2012
# Author: Eldar Abusalimov
# Author: Anton Kozlov
#

ifndef __mybuild_configfile_resource_mk
__mybuild_configfile_resource_mk := 1

include mk/model/resource.mk
include mk/mybuild/configfile-parser.mk

define class-ConfigFileResource
	$(super Resource,$(value 1))

	# Param:
	#   1. File name.
	# Return:
	#   Result of parsing or empry on error.
	$(method loadRootObject : EObject,
		$(for resource <- $(this),
			$(call gold_parse,configfile,$1,
				$(lambda $(set+ resource->issues,
					$(new ParsingIssue,$(resource),$2,$3,$4)))))
	)

endef

# 1. ResourceSet
define configfiles_do_link
	$(invoke $(new Linker).link,$1,$(get mybuild_model_instance->resourceSet))
endef

$(def_all)

endif # __mybuild_configfile_resource_mk

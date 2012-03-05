#
#   Date: Mar 2, 2012
# Author: Eldar Abusalimov
# Author: Anton Kozlov
#

ifndef __mybuild_configfile_resource_mk
__mybuild_configfile_resource_mk := 1

include mk/mybuild/common-resource.mk
include mk/mybuild/configfile-parser.mk

# Constructor args:
#   1. File name.
define class-ConfigFileResource
	$(super GoldParsedResource,$(value 1))

	$(getter goldGrammarName,configfile)

endef

$(def_all)

endif # __mybuild_configfile_resource_mk

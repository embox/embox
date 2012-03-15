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

define class-ConfigFileResourceSet
	$(super ResourceSet,$(value 1))
	$(method createLinker,
		$(new ConfigFileLinker,$(this)))
endef

define config_create_resource_set_from_files
	$(new ResourceSet,$(foreach r,$1,$($r)))
endef

define config_link_with_myfile_model
	$(for rs <- $1,
		myfileSet <- $2,

		$(invoke $(get rs->linker).linkAgainst,$(rs),$(myfileSet))

		$(for r <- $(get rs->resources),
			issue <- $(get r->issues),
			$(invoke issue->report))

		$(rs))
endef

$(def_all)

endif # __mybuild_configfile_resource_mk

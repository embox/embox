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

# Constructor:
#   1. List of resources.
#   2. My-file resource set to link against.
define class-ConfigFileResourceSet
	$(field myfileResourceSet : ResourceSet,$2)
	$(super ResourceSet,$1)
	$(method createLinker,
		$(new ConfigLinker,$(this),$(get-field myfileResourceSet)))
endef

# Params:
#   1. List of resources.
#   2. Myfiles resource set.
define config_create_resource_set
	$(for rs <- $(new ConfigFileResourceSet,$1,$2),

		$(invoke $(get rs->linker).resolveAllLinks)

		$(call myfile_resources_check_optionbind,$(get rs->resources))

		$(silent-for r <- $(get rs->resources),
			$(invoke r->printIssues))

		$(rs))
endef

$(def_all)

endif # __mybuild_configfile_resource_mk

#
#   Date: Jan 21, 2012
# Author: Eldar Abusalimov
#

ifndef __mybuild_myfile_resource_mk
__mybuild_myfile_resource_mk := 1

include mk/mybuild/common-resource.mk
include mk/mybuild/myfile-parser.mk
include mk/mybuild/myfile-linker.mk

# Constructor args:
#   1. File name.
define class-MyFileResource
	$(super GoldParsedResource,$(value 1))

	$(getter goldGrammarName,myfile)

endef

# Constructor:
#   1. List of resources.
define class-MyFileResourceSet
	$(super ResourceSet,$1)
	$(method createLinker,
		$(new MyFileLinker,$(this),$(this)))
endef

define myfile_create_resource_set_from_files
	$(for rs <- $(new MyFileResourceSet,$(for f <- $1,$($f))),

		$(invoke $(get rs->linker).resolveAllLinks)

		$(for r <- $(get rs->resources),
			$(invoke r->printIssues))

		$(rs))
endef

$(def_all)

endif # __mybuild_myfile_resource_mk

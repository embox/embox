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

define myfile_create_resource_set_from_files
	$(for rs <-
			$(new ResourceSet,
				$(for f <- $1,$($f))),

		$(invoke $(new MyFileLinker).link,$(rs))

		$(for r <- $(get rs->resources),
			issue <- $(get r->issues),
			$(invoke issue->report))

		$(rs))
endef

$(def_all)

endif # __mybuild_myfile_resource_mk

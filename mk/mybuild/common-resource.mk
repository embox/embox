#
#   Date: Mar 5, 2012
# Author: Eldar Abusalimov
# Author: Anton Kozlov
#

ifndef __mybuild_common_resource_mk
__mybuild_common_resource_mk := 1

include mk/model/resource.mk
include mk/mybuild/myfile-parser.mk

define class-GoldParsedResource
	$(super Resource,$(value 1))

	# Param:
	#   1. File name.
	# Return:
	#   Result of parsing or empry on error.
	$(method loadRootObject : EObject,
		$(for resource <- $(this),
			$(call gold_parse,$(get goldGrammarName),$1,
				$(lambda $(set+ resource->issues,
					$(new ParsingIssue,$(resource),$2,$3,$4)))))
	)

	$(property goldGrammarName)

endef

# Constructor args:
#   1. Resource.
#   2. Severity.
#   3. Location.
#   4. Message.
define class-ParsingIssue
	$(super BaseIssue,$1,$2,$3,$4)
endef

$(def_all)

endif # __mybuild_common_resource_mk

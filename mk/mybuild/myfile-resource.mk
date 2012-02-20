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
					$(new ParsingIssue,$(resource),$2,$3,$4)))))
	)

endef

# Constructor args:
#   1. Resource.
#   2. Severity.
#   3. Location.
#   4. Message.
define class-ParsingIssue
	$(super BaseIssue,$1,$2,$3,$4)
endef

define class-MyBuildGlobalState
	$(super CompositeLinkageUnit)

	$(getter children,
		$(get resources))

	$(property-field resources... : MyFileResource,$1)

	# Param:
	#   1. The resource.
	$(method getResourceImportNormalizers,
		$(for root <- $(get 1->rootObject),
			$(with $(get root->name),
				$(if $1,
					$(assert $(singleword [$1]))
					$1.)%)))

	$(invoke link)

	$(for l <- $(get unresolvedLinks),
		$(set+ $(get l->eResource).issues,
			$(new UnresolvedLinkIssue,$l)))

	$(for r <- $(get resources),
		issue <- $(get r->issues),
		$(invoke issue->report))
endef

$(def_all)

endif # __mybuild_myfile_resource_mk

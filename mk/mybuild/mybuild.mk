#
#   Date: Feb 10, 2012
# Author: Eldar Abusalimov
#

ifndef __mybuild_mybuild_mk
__mybuild_mybuild_mk := 1

include mk/mybuild/myfile-resource.mk

# Constructor args:
#   1. List of resource objects.
define class-Mybuild
	$(property-field resourceSet : ResourceSet,$(new ResourceSet,$1))

	# Param:
	#   1. The resource.
	$(method getResourceImportNormalizers,
		$(for root <- $(get 1->contentsRoot),
			$(with $(get root->name),
				$(if $1,
					$(assert $(singleword [$1]))
					$1.)%)))

	$(invoke $(new Linker).link,$(get resourceSet))

	$(for r <- $(get $(get resourceSet).resources),
		l <- $(get r->unresolvedLinks),
		$(set+ $(get l->eResource).issues,
			$(new UnresolvedLinkIssue,$l)))

	$(for r <- $(get $(get resourceSet).resources),
		issue <- $(get r->issues),
		$(invoke issue->report))
endef

mybuild_model_instance = $(__mybuild_model_instance)

$(def_all)

endif # __mybuild_mybuild_mk

#
#   Date: Feb 10, 2012
# Author: Eldar Abusalimov
#

ifndef __mybuild_mybuild_mk
__mybuild_mybuild_mk := 1

ifeq (1,0)

include mk/mybuild/myfile-resource.mk

# Constructor args:
#   1. List of resource objects.
define class-Mybuild
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

else

include mk/mybuild/resource.mk

# Constructor args:
#   1. (Optional) List of resource objects.
define class-mybuild

	$(property-field resources... : resource)

	$(property __unresolved_links... : module_link)
	$(getter __unresolved_links,
		$(for r <- $(get resources),
			o <- $(get r->exports),
			f <- depends_refs super_module_ref,
			l <- $(get o->$f),
				$(if $(not $(invoke $l.resolved?)),$l))
	)

	# Takes a list of resource objects and links them together.
	$(method link,
		$(set-field resources,$(suffix $1))
		$(silent-for \
			l <- $(get __unresolved_links),
			r <- $(get resources),
			m <- $(filter $(get l->link_name).%,$(get r->exports)),
				$(if $(eq $(basename $m),$(get l->link_name)),
					$(invoke l->resolve,$m)))
	)

	$(if $(value 1),$(invoke link,$1))
endef

endif

mybuild_model_instance = $(__mybuild_model_instance)

$(def_all)

endif # __mybuild_mybuild_mk

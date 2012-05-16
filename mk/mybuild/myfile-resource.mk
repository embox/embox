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

# Params:
#   1. List of resources.
define myfile_create_resource_set
	$(for rs <- $(new MyFileResourceSet,$1),

		$(invoke $(get rs->linker).resolveAllLinks)

		$(call myfile_resources_check_optionbind,$(get rs->resources))

		$(silent-for \
			res <- $1,
			obj <- $(get res->contents),
			$(if $(invoke MyFile_AnnotationTarget->isInstance,$(obj)),
				$(call mybuild_annotation_process,MyLink,$(obj),$(res))))

		$(for r <- $(get rs->resources),
			$(invoke r->printIssues))

		$(rs))
endef

# Check OptionBinding's for type correctness in resources of any type
# Params:
#   1. List of resources.
define myfile_resources_check_optionbind
	$(for \
		resource <- $1,
		containment <- $(get resource->contents),
		optionBind <- $(invoke containment->eContentsOfType,
			$(MyFile_OptionBinding)),
		opt <- $(get optionBind->option),
		val <- $(get optionBind->value),

		$(if $(invoke opt->validateValue,$(val)),,
			$(invoke $(get optionBind->eResource).addIssues,
				$(new BaseIssue,$(get optionBind->eResource),
					error,,	Option $(get opt->qualifiedName) \
						is assigned to incompatible type))))
endef

$(def_all)

endif # __mybuild_myfile_resource_mk

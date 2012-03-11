#
#   Date: Feb 10, 2012
# Author: Eldar Abusalimov
#

ifndef __mybuild_mybuild_mk
__mybuild_mybuild_mk := 1

include mk/mybuild/myfile-resource.mk

# Constructor args:
#   1. Myfile resource set
#   2. Config resource set
define class-Mybuild
	$(property-field myfileResourceSet : ResourceSet)
	$(property-field configResourceSet : ResourceSet)

	$(set myfileResourceSet,$(firstword $1))
	$(set configResourceSet,$(word 2,$1))

	$(map moduleInstanceStore... : BuildModuleInstance)

	# Args:
	$(method createBuild,
		$(for build <-$(new BuildBuild),
			$(set build->modules,
				$(invoke checkResolve,$(for \
					configResSet <- $(get configResourceSet),
					config <- $(get configResSet->resources),
					cfgFileContent <- $(get config->contentsRoot),
					cfgConfiguration <- $(firstword $(get cfgFileContent->configurations)), #FIXME
					cfgInclude <- $(get cfgConfiguration->includes),
					module <- $(get cfgInclude->module),

					$(invoke moduleInstanceClosure,$(module)))))
			$(build)))


	# Args:
	#  1. List of moduleInstance's
	$(method checkResolve,
		$(for inst<-$1,
			isAbstr<-$(get $(get inst->type).isAbstract),
			$(if $(singleword $(get inst->depends)),,
				$(error $(get $(get inst->type).qualifiedName) api realize error, move to Issues)))
		$1)


	# Args:
	#  1. MyModule object instance
	#  2. (Optional) Configuration
	# Return:
	#  ModuleInstance instance
	$(method moduleInstance,
		$(for mod <- $1,
			$(or $(map-get moduleInstanceStore/$(mod)),
				$(for moduleInstance <-
					$(new BuildModuleInstance,$(mod),
						$(if $(value 2),$2)),
					$(map-set moduleInstanceStore/$(mod),
						$(moduleInstance))
					$(for super <- $(get mod->allSuperTypes),
						superInst <- $(invoke moduleInstance,$(super)),
						$(set+ superInst->depends,$(moduleInstance)))
					$(moduleInstance)))))


	# Args:
	#  1. MyModule object instance
	# Return:
	#  List of ModuleInstance for module and it's depends #TODO and supertypes
	$(method moduleInstanceClosure,
		$(for thisInst<-$(invoke moduleInstance,$1),
			$(thisInst) \
			$(for \
				mod <- $1,
				dep <- $(get mod->depends),
				depInst <- $(invoke moduleInstanceClosure,$(dep)),
				$(set+ thisInst->depends,$(depInst))
				$(depInst))))

endef

define printInstances
		$(strip $(for mybuild<-$1,
			inst<-$(get mybuild->modules),
			$(warning $(get $(strip $(get inst->type)).qualifiedName))
			$(for dep <- $(get inst->depends),
				$(warning $(\t)$(get $(get dep->type).qualifiedName)))))
endef

# Args
#  1. Mybuild instance
define Mybuild-createBuild
	$(invoke $1.createBuild)
endef

$(def_all)

endif # __mybuild_mybuild_mk

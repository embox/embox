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
				$(for \
			       		configResSet <- $(get configResourceSet),
					config <- $(get configResSet->resources),
					cfgFileContent <- $(get config->contentsRoot),
					cfgConfiguration <- $(firstword $(get cfgFileContent->configurations)), #FIXME
					cfgInclude <- $(get cfgConfiguration->includes),
					module <- $(get cfgInclude->module),

					$(invoke moduleInstanceClosure,$(module))))
			$(build)))


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
					$(moduleInstance)))))
	$(method listInstances,
		$(for i <- modules,
			$(get i->name)))
	# Args:
	#  1. MyModule object instance
	# Return:
	#  List of ModuleInstance for module and it's depends #TODO and supertypes
	$(method moduleInstanceClosure,
		$(invoke moduleInstance,$1) \
		$(for mod <- $1,
			dep <- $(get mod->depends),
			$(invoke moduleInstanceClosure,$(dep))))

endef

# Args
#  1. Mybuild instance
define Mybuild-createBuild
	$(invoke $1.createBuild)
endef

$(def_all)

endif # __mybuild_mybuild_mk

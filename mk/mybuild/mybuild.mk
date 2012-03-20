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
				$(invoke getBuildModules))
			$(build)))

	$(method getBuildModules,
		$(with \
			$(for \
				configResSet <- $(get configResourceSet),
				config <- $(get configResSet->resources),
				cfgFileContent <- $(get config->contentsRoot),
				cfgConfiguration <- $(firstword $(get cfgFileContent->configurations)), #FIXME
				cfgInclude <- $(get cfgConfiguration->includes),
				module <- $(get cfgInclude->module),
					$(invoke moduleInstanceClosure,$(module))),

			$(if $(invoke checkResolve,$1),,$1)))


	# Args:
	#  1. List of moduleInstance's
	$(method checkResolve,
		$(strip
			$(for inst<-$1,
				isAbstr<-$(get $(get inst->type).isAbstract),
				$(if $(singleword $(get inst->depends)),,
					$(error $(get $(get inst->type).qualifiedName) api realize error, move to Issues $(get inst->depends))
					$(inst)))))


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
	#  List of ModuleInstance for module and it's depends
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
		$(strip $(for buildBuild<-$1,
			inst<-$(get buildBuild->modules),
			$(warning $(get $(strip $(get inst->type)).qualifiedName))
			$(warning Deps:)
			$(for dep <- $(get inst->depends),
				$(warning $(\t)$(get $(get dep->type).qualifiedName)))
			$(warning Opts:)
			$(for opt <- $(get $(get inst->type).allOptions),
				val <-$(warning $(\t)$(get opt->name))$(get opt->defaultValue),
				$(warning $(\t)$(\t)DefVal : $(get val->value)))))
endef

define listInstances
		$(strip $(for buildBuild<-$1,
			$(get buildBuild->modules)))
endef

# Args
#  1. Mybuild instance
define Mybuild-createBuild
	$(invoke $1.createBuild)
endef

$(def_all)

endif # __mybuild_mybuild_mk

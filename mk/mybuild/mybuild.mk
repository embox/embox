#
#   Date: Feb 10, 2012
# Author: Eldar Abusalimov
#

ifndef __mybuild_mybuild_mk
__mybuild_mybuild_mk := 1

include mk/mybuild/myfile-resource.mk

# Constructor args:
#   1. Configuration resource set.
define class-Mybuild
	$(property-field configResourceSet : ResourceSet,$1)

	$(map moduleInstanceStore... : BuildModuleInstance)

	$(map activeFeatures... : BuildModuleInstance)

	# Args:
	$(method createBuild,
		$(for \
			build <-$(new BuildBuild),
			issueReceiver <- $(new IssueReceiver),

			$(set-field build->issueReceiver,$(issueReceiver))

			$(set build->modules,
				$(invoke getBuildModules))

			$(for modInst <- $(get build->modules),
				mod <- $(get modInst->type),
				$(map-set build->moduleInstanceByName/$(get mod->qualifiedName),
					$(modInst)))

			$(build)))

	$(method getBuildModules,
		$(with \
			$(for \
				configResSet <- $(get configResourceSet),
				config <- $(get configResSet->resources),
				cfgFileContent <- $(get config->contentsRoot),
				cfgConfiguration <- $(firstword $(get cfgFileContent->configurations)),#FIXME
				cfgInclude <- $(get cfgConfiguration->includes),
				module <- $(get cfgInclude->module),

				$(if $(invoke moduleInstanceHas,$(module)),,
					$(invoke moduleInstanceClosure,$(module)))

				$(for moduleInst <- $(invoke moduleInstance,$(module)),
					$(set moduleInst->includeMember,$(cfgInclude)))),

			$(invoke superSetDeps,$1)
			$(invoke checkAbstractRealization,$1)
			$(invoke checkFeatureRealization,$1)
			$(invoke optionBind,$1)
			$1))


	$(method superSetDeps,
		$(silent-for \
			inst <- $1,
		   	mod <- $(get inst->type),
			super <- $(get mod->allSuperTypes),
			superInst <- $(invoke moduleInstanceHas,$(super)),
			$(set+ superInst->depends,$(inst))))


	$(method getInstDepsOrigin,
		$(for \
			inst <- $1,
			instType <- $(get inst->type),
			$(if $(get inst->includeMember),
				$(for \
					includeMember <- $(get inst->includeMember),
					resource <- $(get includeMember->eResource),
						$(\n)$(\n)$(\t)$(get resource->fileName):
							$(get includeMember->origin) \
							$(get instType->qualifiedName)),
					$(\n)$(\n)$(\t)(As dependence)::
						$(get instType->qualifiedName))))


	# Args:
	#  1. List of moduleInstance's
	$(method checkAbstractRealization,
		$(for inst <- $1,
			instType <- $(get inst->type),
			isAbstr<-$(get instType->isAbstract),
			$(if $(singleword $(get inst->depends)),,#Not single realization, error
				$(if $(get inst->depends),
					$(invoke issueReceiver->addIssues,$(new InstantiateIssue,,error,,
						Multiplie abstract realization: $(get instType->qualifiedName)
							$(invoke getInstDepsOrigin,$(get inst->depends)))),
					$(invoke issueReceiver->addIssues,$(new InstantiateIssue,,error,,
						No abstract realization: $(get instType->qualifiedName))))))
		)

	$(method checkFeatureRealization,
		$(for inst <- $1,
			instType <- $(get inst->type),
			require <- $(get instType->requires),
			$(if $(map-get activeFeatures/$(require)),#OK nothing to do
				,#Feature not realized, error
				$(invoke issueReceiver->addIssues,$(new InstantiateIssue,,error,,
					Feature $(get require->qualifiedName) required by
					$(get instType->qualifiedName) is not implemented)))))


	# Args:
	#  1. List of moduleInstance's
	$(method optionBind,
		$(for \
			modInst <- $1,
			mod <- $(get modInst->type),
			isAbstract <- $(if $(get mod->isAbstract),,false),
			opt<-$(get mod->allOptions),
			optValue <- $(or $(if $(get modInst->includeMember),# if module was explicitly enabled
										# from configs and probably has
										# config option bindings
						$(invoke findOptValue,$(opt),
							$(get $(get modInst->includeMember).optionBindings))),
					 $(get opt->defaultValue),
					 Error),

			$(if $(eq $(optValue),Error),
				$(invoke issueReceiver->addIssues,$(new InstantiateIssue,
					$(for includeMember <- $(get modInst->includeMember),
						$(get includeMember->eResource)),
					warning,
					$(for includeMember <- $(get modInst->includeMember),
						$(get includeMember->origin)),
					Couldn't bind option $(get opt->name) in module \
						$(get mod->qualifiedName) to a value))
					,

				$(silent-for optInst <- $(new BuildOptionInstance),
					$(set optInst->option,$(opt))
					$(set optInst->optionValue,$(optValue))
					$(set+ modInst->options,$(optInst))))))

	$(method findOptValue,
		$(for \
			opt <- $1,
			optName <- $(get opt->name),
			optBinding <- $2,
			optBindOpt <- $(get optBinding->option),
			optBindName <- $(get optBindOpt->name),
			optBindVal <- $(get optBinding->optionValue),
			$(if $(and $(eq $(optName),$(optBindName)),
					$(invoke opt->validateValue,$(optBindVal))),
				$(optBindVal))))


	# Args:
	#  1. MyModule instance
	# Return:
	#  ModuleInstance instance
	$(method moduleInstance,
		$(for mod <- $1,
			$(or $(invoke moduleInstanceHas,$(mod)),
				$(for moduleInstance <-
					$(new BuildModuleInstance,$(mod)),

					$(set moduleInstance->type,$(mod))

					$(map-set moduleInstanceStore/$(mod),
						$(moduleInstance))

					$(for provide <- $(get mod->provides),
						opt <- $(provide) $(get provide->allSubFeatures),
						$(map-set+ activeFeatures/$(opt),
							$(moduleInstance)))

					$(moduleInstance)))))

	# Args:
	#  1. MyModule instance
	# Return:
	#  ModuleInstance on positive
	#  None on negative
	$(method moduleInstanceHas,
		$(map-get moduleInstanceStore/$1))

	# Args:
	#  1. MyModule object instance, that not presented in build
	# Return:
	#  List of ModuleInstance for module, that have no reperesents yet
	$(method moduleInstanceClosure,
			$(for thisInst<-$(invoke moduleInstance,$1),
				$(thisInst) \
				$(for \
					mod <- $1,
					dep <- $(get mod->depends),
					was <- was$(invoke moduleInstanceHas,$(dep)),
					$(for depInst <- $(invoke moduleInstance,$(dep)),
						$(if $(filter $(depInst),$(get thisInst->depends)),,
							$(set+ thisInst->depends,$(depInst))))
					$(if $(filter was,$(was)),
						$(invoke moduleInstanceClosure,$(dep))))))

endef

define mybuild_create_build
	$(invoke $(new Mybuild,$(__config_resource_set)).createBuild)
endef

define printInstance
	$(for inst<-$1,
		mod <- $(get inst->type),
		--- $(get mod->qualifiedName) ---$(\n)
		Inclusion reason: $(if $(get inst->includeMember),explicit,as dependence)$(\n)
		Depends:$(\n)
		$(for dep <- $(get inst->depends),
			$(\t)$(get $(get dep->type).qualifiedName)$(\n))
		Dependents:$(\n)
		$(for dep <- $(get inst->dependent),
			$(\t)$(get $(get dep->type).qualifiedName)$(\n))
		OptInsts:$(\n)
		$(for optInst <- $(get inst->options),
			opt <- $(get optInst->option),
			val <-$(get optInst->optionValue),
			$(\t)$(get opt->name) : $(get val->value)$(\n))
		Sources:$(\n)
		$(for srcMember <- $(get mod->sourcesMembers),
				src <- $(get srcMember->files),
	       		$(for annot<-$(get srcMember->annotations),
					annotType <- $(get annot->type),
					annotName <- $(get annotType->qualifiedName),
					annotBind <- $(get annot->bindings),
					@$(annotName):$(\n)
					$(for opt <- $(get annotBind->option),
						optName <- $(get opt->name),
						optValue <- $(get $(get annotBind->optionValue).value),
						$(\t)$(optName) = $(optValue)$(\n)))
				$(get src->fileFullName)$(\n))
	)
endef

define printInstances
	$(for buildBuild<-$1,
		$(warning $(call printInstance,$(get buildBuild->modules)))))
endef

define printIssues
	$(for issueReceiver <- $(get-field 1->issueReceiver),
		$(if $(invoke issueReceiver->getIssues),
			$(invoke issueReceiver->printIssues)))
endef

define listInstances
		$(strip	$(get 1->modules))
endef

define class-InstantiateIssue
	$(super BaseIssue,$(value 1),$(value 2),$(value 3),$(value 4))

endef

$(def_all)

endif # __mybuild_mybuild_mk

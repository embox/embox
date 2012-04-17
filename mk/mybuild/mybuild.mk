#
#   Date: Feb 10, 2012
# Author: Eldar Abusalimov
#

ifndef __mybuild_mybuild_mk
__mybuild_mybuild_mk := 1

include mk/mybuild/myfile-resource.mk

# Constructor args:
#   1. Configuration.
define class-Mybuild
	$(field configuration : CfgConfiguration,$1)

	$(map moduleInstanceStore... : CfgModuleInstance) #by module
	$(map activeFeatures... : CfgModuleInstance) #by feature

# Public:

	# Create BuildModel from current state.
	#
	# Return:
	#	Created BuildModel with issuees
	$(method createBuild,
		$(for \
			build <-$(new CfgBuild),
			issueReceiver <- $(new IssueReceiver),

			$(set-field build->issueReceiver,$(issueReceiver))

			$(set build->modules,
				$(invoke getBuildModules))

			$(for modInst <- $(get build->modules),
				mod <- $(get modInst->type),
				super <- $(mod) $(get mod->allSuperTypes),
				$(map-set build->moduleInstanceByName/$(get super->qualifiedName),
					$(modInst)))

			$(build)))

# Private:

	# Gets all modulesInstance's created according configResourceSet
	#
	# Context:
	#   'issueReceiver'
	# Args:
	# Return:
	#	List of avaible moduleInstances. Some of them may not be created, issue was created
	$(method getBuildModules,
		$(with \
			$(for \
				cfgInclude <- $(get $(get-field configuration).includes),
				module <- $(get cfgInclude->module),

				$(if $(invoke moduleInstanceHas,$(module)),,
					$(invoke moduleInstanceClosure,$(module)))

				$(for moduleInst <- $(invoke moduleInstance,$(module)),
					$(set moduleInst->includeMember,$(cfgInclude)))),

			$(invoke checkFeatureRealization,$1)
			$(invoke optionBind,$1)
			$1))

	# Helper method, returns string representation of moduleInstance origin
	#
	# Args:
	#	1. ModuleInstance
	# Return:
	#	String representation of ModuleInstance origin
	$(method getInstDepsOrigin,
		$(for \
			inst <- $1,
			instType <- $(get inst->type),
			$(if $(get inst->includeMember),
				$(for \
					includeMember <- $(get inst->includeMember),
					resource <- $(get includeMember->eResource),
						$(get instType->qualifiedName)(
						$(get resource->fileName):
						$(get includeMember->origin))),
				$(get instType->qualifiedName)(As dependence))))


	# Checks that ModuleInstances have all feature realization in current build
	#
	# Context:
	#	IssueReceiver
	# Args:
	#	1. List of ModuleInstance
	# Return:
	#   None.
	# Side effect:
	#	Probably issue was added to IssueReceiver
	$(method checkFeatureRealization,
		$(for inst <- $1,
			instType <- $(get inst->type),
			require <- $(get instType->requires),
			$(if $(map-get activeFeatures/$(require)),#OK nothing to do
				,#Feature not realized, error
				$(invoke issueReceiver->addIssues,$(new InstantiateIssue,,error,,
					Feature $(get require->qualifiedName) required by
					$(get instType->qualifiedName) is not implemented)))))


	# Bind options for ModuleInstances (set ModuleInstance optionBinding field with option binding)
	#
	# Context:
	#	Issue Receiver
	# Args:
	#  1. List of ModuleInstance
	# Return:
	#   None.
	# Side effect:
	#	Probably issue was added to IssueReceiver
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

				$(silent-for optInst <- $(new CfgOptionInstance),
					$(set optInst->option,$(opt))
					$(set optInst->optionValue,$(optValue))
					$(set+ modInst->options,$(optInst))))))

	# Find option binding for option within list.
	#
	# Args:
	#	1. Option
	#	2. List of OptionBinding
	# Return:
	#	OptionBindings for given option
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

	# Effectivelly allocate ModuleInstance.
	# If there are ModuleInstance for module supertype, it will be returned,
	# else new ModuleInstance returned.
	#
	# Returned ModuleInstance is not initialized.
	#
	# Context:
	#	IssueReceiver
	#	cfgInclude. IncludeMember of processing ModuleInstance requiest.
	#	mod. Module for which ModuleInstance is creating.
	# Args:
	#	None.
	# Return:
	#	Uninitialized ModuleInstance
	# Side effect:
	#	Probably issue was added in case mod has incompatible hierarchy
	#	sibiling (sibiling extends one of mod supertype but nor extends
	#	mod, neither is one of mods subtype).
	$(method moduleInstanceSuper,
		$(with \
			$(sort \
				$(for super <- $(mod) $(get mod->allSuperTypes),
					$(invoke moduleInstanceHas,$(super)))),
			$(if $1,
				$(if $(filter $(get 1->type),$(mod) $(get mod->allSuperTypes)),
					$1,
					$(if $(filter $(get 1->type),$(get mod->allSubTypes)),,
						$(invoke issueReceiver->addIssues,$(new InstantiateIssue,
							$(for includeMember <- $(cfgInclude),
								$(get includeMember->eResource)),
							error,
							$(for includeMember <- $(cfgInclude),
								$(get includeMember->origin)),
							Module $(get mod->qualifiedName) extends module supertype \
								already extended by incompatible $(invoke getInstDepsOrigin,$1)))
					)),
				$(new CfgModuleInstance,$(mod)))))

	# Get ModuleInstance for Module
	#
	# Args:
	#	1. Module
	# Return:
	#	ModuleInstance instance
	# SideEffect:
	#	Build realized features appends Module features
	$(method moduleInstance,
		$(for \
			mod <- $1,
			moduleInstance <- $(call Mybuild.moduleInstanceSuper),

			$(set moduleInstance->type,$(mod))

			$(for super <- $(mod) $(get mod->allSuperTypes),
				$(map-set moduleInstanceStore/$(super),
					$(moduleInstance)))

			$(for provide <- $(get mod->provides),
				opt <- $(provide) $(get provide->allSubFeatures),
				$(map-set+ activeFeatures/$(opt),
					$(moduleInstance)))

			$(moduleInstance)))

	# Check if there are ModuleInstance for Module
	#
	# Args:
	#  1. Module
	# Return:
	#  ModuleInstance on positive
	#  None on negative
	$(method moduleInstanceHas,
		$(map-get moduleInstanceStore/$1))

	# Get ModuleInstance closure of given  Module
	#
	# Args:
	#  1. Module, that not presented in build
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

define mybuild_get_active_build
	$(for config <- $(get __config_resource_set->resources),
		cfgFileContent <- $(get config->contentsRoot),
		cfgConfiguration <- $(firstword $(get cfgFileContent->configurations)),#FIXME
		$(get cfgConfiguration->build))
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

#
#   Date: Feb 10, 2012
# Author: Eldar Abusalimov
#

ifndef __mybuild_mybuild_mk
__mybuild_mybuild_mk := 1

include mk/mybuild/myfile-resource.mk


include_checkers_list :=
include_past_checkers := \

builders_list := \
	Mybuild.closeInstances \
	Mybuild.specifyInstances \
	Mybuild.optionBind

checkers_list := \
	Mybuild.checkAbstractRealization \
	Mybuild.checkFeatureRealization \
	Mybuild.checkCyclicDependency \
	Mybuild.optionCheckUnique \
	Mybuild.optionCheckConstraints


# Args:
# 	1. Cheking instances
# 	2. Checkers list
# 	3. Passed checkers list
define check
	$(silent-for \
		inst <- $1,
		checker <- $2,
		$(call $(checker),$(inst)))
endef

define getAnnotationTarget
	$(for target<-$1,
		$(call getAnnotation,$(get target->annotations),$2,$(value 3)))
endef

define getAnnotation
	$(strip $(for annot <- $1,
		annotType<-$(get annot->type),
		$(if $(eq $(get annotType->qualifiedName),$2),
			$(if $(value 3),
				$(for bind <- $(get annot->bindings),
					$(if $(eq $3,$(get $(get bind->option).name)),
						$(get bind->value))),
				$2))))

endef

define selectUnique
	$(if $2,
		$(if $(filter $(firstword $2),$1),
			$(call $0,$1,$(wordlist 2,$(words $2),$2)),
			$(call $0,$1 $(firstword $2),$(wordlist 2,$(words $2),$2))),
		$1)
endef

# Constructor args:
#   1. Configuration.
define class-Mybuild
	$(map moduleInstanceStore : ModuleInstance) #by module
	$(map activeFeatures... : ModuleInstance) #by feature
	$(map includingInstances : MyModuleType) #stack for cyclic detection
	$(map includingInstancesChecked : MyModuleType) #stack for cyclic detection
	$(map recommendations : MyModuleType)
	$(property-field issueReceiver)

# Public:

	# Creates build model from the configuration.
	#
	# Args:
	#   1. Configuration.
	# Return:
	#   New instance of CfgBuild.
	$(method createBuild,
		$(assert $(singleword $1))
		$(for \
			configuration <- $1,
			build <-$(new Build),
			issueReceiver <- $(new IssueReceiver),

			$(set build->configuration,$(configuration))

			$(set-field build->issueReceiver,$(issueReceiver))
			$(set issueReceiver,$(issueReceiver))

			$(invoke includeMandatoryModules)
			$(invoke includeAllBuildModules)

			$(silent-for f <- $(builders_list),
				$(set-field+ current_builders_list,$f)
				$(call $(f),$(invoke listBuildModules)))

			$(if $(call check,$(invoke listBuildModules),
				$(checkers_list)),)

			$(set build->modules,
				$(invoke listBuildModules))

			$(build))
	)

# Private:

	$(field current_builders_list)

	$(method annotationProcess,
		$(call mybuild_annotation_process,Build,$1,$(this)))

	$(method includeModule,
		$(for \
			mod <- $1,
			was <- was$(map-get moduleInstanceStore/$(mod)),
			mod <- $(if $(filter was,$(was)),
						$(invoke annotationProcess,$(mod)),
						$(mod)),
			inst <- $(invoke moduleInstance,$(mod)),

			$(if $(or $(filter was,$(was)),$(filter force,$(value 2))),
				$(call check,$(inst),$(get-field current_builders_list)))
			$(inst)))

	$(method includeMandatoryModules,
		$(silent-for \
			mandatory <- $(call mybuild_resolve_or_die,mybuild.lang.Mandatory),
			module <- $(get $(invoke $(invoke __myfile_resource_set->
				resources>contentsRoot>eContentsOfType,
				$(MyFile_ModuleType))
					.getAnnotationsOfType,$(mandatory))
						.target),
			$(invoke moduleInstance,$(module))))

	# Gets all modulesInstance's created according configResourceSet
	#
	# Context:
	#   'configuration'
	#   'issueReceiver'
	$(method includeAllBuildModules,
		$(silent-for \
			cfgUnverifiedInclude <- $(get configuration->includes),
			cfgInclude <- $(invoke annotationProcess,$(cfgUnverifiedInclude)),
			module <- $(invoke annotationProcess,$(get cfgInclude->module)),
			inst <- $(invoke moduleInstance,$(module)),
			$(set inst->includeMember,$(cfgInclude))))

	$(method listBuildModules,
		$(call selectUnique,,
			$(for mod <- $(get-field moduleInstanceStore),
				$(map-get moduleInstanceStore/$(mod)))))

	$(method addIssue,
		$(invoke $(get issueReceiver).addIssues,$(new InstantiateIssue,
			$1,$2,$3,$4)))

	$(method addIssueGlobal,
		$(invoke addIssue,,$1,,$2))

	$(method addIssueInclude,
			$(invoke addIssue,
				$(for includeMember <- $1,
					$(get includeMember->eResource)),
				$2,
				$(for includeMember <- $1,
					$(get includeMember->origin)),
				$3))

	$(method addIssueInstance,
		$(invoke addIssueInclude,$(get 1->includeMember),$2,$3))

	$(method addRecommendationRule,
		$(map-set recommendations/$1,$2))

	$(method getRecommendation,
		$(map-get recommendations/$1))

	$(method closeInstances,
		$(for inst <- $1,
			$(invoke instanceClosure,$(inst))))

	$(method specifyInstances,
		$(for \
			inst <- $1,
			targetModule<-$(map-get recommendations/$(get inst->type)),
			$(if $(invoke targetModule->isSubTypeOf,$(get inst->type)),
				$(invoke includeModule,$(targetModule)))))

	# Cheker for abstract realization. If there is only one subtype of given
	# abstract module, it will included to build with all dependencies, which
	# are also checked for abstract realization with `check' restart
	#
	# Args:
	#	1. List of ModuleInstance
	#	2. Checkers, which ModuleInstances was passed.
	$(method checkAbstractRealization,
		$(for \
			inst <- $1,
			instType <- $(get inst->type),
			$(with \
				$(if $(get instType->isAbstract),
					$(if $(singleword $(get instType->subTypes)),
							$(call check,
								$(invoke moduleInstance,
									$(get instType->subTypes)),
								$2 $0,)
							$(if $(get inst->type>isAbstract),
								1,
								),
					 	1),
				 ),


				$(if $(strip $1),
					$(invoke addIssueGlobal,error,
							No abstract realization: \
								$(get instType->qualifiedName))))))

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
	#	2. Checkers, which ModuleInstances was passed.
	# Return:
	#   None.
	# Side effect:
	#	Probably issue was added to IssueReceiver
	$(method checkFeatureRealization,
		$(for inst <- $1,
			instType <- $(get inst->type),
			$(if $(strip \
				$(for require <- $(get instType->requires),
					$(if $(map-get activeFeatures/$(require)),#OK
						,error#Feature not realized, error
						$(invoke addIssueGlobal,error,
							Feature $(get require->qualifiedName) required by \
								$(get instType->qualifiedName) is not
								implemented)))),
			)))

	# Bind options for ModuleInstances (set ModuleInstance optionBinding field
	# with option binding)
	#
	# Context:
	#	Issue Receiver
	# Args:
	#	1. List of ModuleInstance
	#	2. Checkers, which ModuleInstances was passed.
	# Return:
	#   None.
	# Side effect:
	#	Probably issue was added to IssueReceiver
	$(method optionBind,
		$(for \
			modInst <- $1,
			mod <- $(get modInst->type),
			$(if $(strip \
				$(for \
					opt<-$(get mod->allOptions),
					optValue <- $(or $(if $(get modInst->includeMember),
											# if module was explicitly enabled
											# from configs and probably has
											# config option bindings
										$(invoke findOptValue,$(opt),
											$(get $(get modInst->includeMember).
														optionBindings))),
								 $(get opt->defaultValue),),
					$(if $(optValue),
						$(silent-for optInst <- $(new OptionInstance),
							$(set optInst->option,$(opt))
							$(set optInst->value,$(optValue))
							$(set+ modInst->options,$(optInst))),
						$(invoke addIssueInstance,$(modInst),warning,
							Could not bind option $(get opt->name) in module \
								$(get mod->qualifiedName) to a value)Error)))
				,,)))

	$(map optionUniq)
	$(map optionSet)

	$(method optionCheckUniqueMark,
		$(if $(and $1,$2),
			$(map-set optionUniq/$(get 1->value),1),
			$(if $2,
				$(invoke addIssueInstance,
					$(inst),
					warning,
					$(get optType->qualifiedName) has @Unique without @Type))
		1))

	$(method optionCheckUniqueCheck,
			$(if $1,
				$(for \
					inst<-$3,
					typeId<-$(get 1->value),
					valueMark <- $(subst $(\s),_,
						$(get $(get opt->value).value)),
					setMark<-$(typeId)_set_$(valueMark),
					$(if $(map-get optionUniq/$(typeId)),
						$(for anotherOpt<-$(map-get optionSet/$(setMark)),
							Error
							$(invoke addIssueInstance,$(inst),error,
								Unique type $(typeId) assigned many times \
								to same value in \
								$(get optType->qualifiedName) \
								(first assing occured in \
								$(get $(get anotherOpt->option).qualifiedName) \
								inclusion)
							))
						$(map-set optionSet/$(setMark),$(opt))))))

	$(method optionCheckUnique,
		$(for phase <- Mybuild.optionCheckUniqueMark \
						Mybuild.optionCheckUniqueCheck,
			$(with $1,$(for inst <- $1,
				opt<-$(get inst->options),
				optType<-$(get opt->option),
				member<-$(invoke optType->eContainer),
				$(with \
					$(call getAnnotationTarget,
						$(member),
						mybuild.lang.Type,
						value),
					$(call getAnnotationTarget,
						$(member),
						mybuild.lang.Unique),
					$(inst),

					$($(phase)))),

				$(if $(strip $2),,))))

	$(method optionCheckConstraints,
		$(with $1,$(for inst<-$1,
				opt<-$(get inst->options),
				optVal <- $(get opt->value),
				optValRaw <- $(get optVal->value),
				optType<-$(get opt->option),
				member<-$(invoke optType->eContainer),
				$(if $(invoke MyFile_NumberOption->isInstance,$(optType)),
					$(with \
						$(for cmp <- ge gt le lt,
							val <- $(call getAnnotationTarget,
										$(member),
										mybuild.lang.NumConstraint,
										$(cmp)),
					$(if \
						$(shell if [ ! $(optValRaw) -$(cmp) $(get val->value) ]; \
							then echo 1; fi),
						$(invoke addIssueInstance,$(inst),error,option $(get optType->qualifiedName) \
							constraint check error)))))),

			$(if $(strip $2),
				,
				$1)))

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
			optBindVal <- $(get optBinding->value),
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
					$(map-get moduleInstanceStore/$(super)))),
			$(if $1,
				$(if $(filter $(get 1->type),$(mod) $(get mod->allSuperTypes)),
					$1,
					$(if $(filter $(get 1->type),$(get mod->allSubTypes)),$1,
						$(invoke addIssueInclude,
							$(cfgInclude),
							error,
							Module $(get mod->qualifiedName) extends module \
								supertype already extended by incompatible \
								$(invoke getInstDepsOrigin,$1)))
					),
				$(new ModuleInstance,$(mod)))))

	$(method setInstanceToType,
		$(set 1->type,$2)

		$(for super <- $2 $(get 2->allSuperTypes),
			$(map-set build->moduleInstanceByName/$(get super->qualifiedName),
				$1)
			$(map-set moduleInstanceStore/$(super),
				$1))

			$(for provide <- $(get 2->provides),
				opt <- $(provide) $(get provide->allSubFeatures),
				$(map-set+ activeFeatures/$(opt),
					$1)))


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
			moduleInstance<-$(call Mybuild.moduleInstanceSuper),

			$(if $(or $(if $(get moduleInstance->type),,1),
						$(invoke $(get moduleInstance->type).isSuperTypeOf,
							$(mod))),
				$(invoke setInstanceToType,$(moduleInstance),$(mod)))

			$(moduleInstance)))

	$(method chkCyclic,
		$(if $(map-get includingInstances/$1),
			$(if $(invoke addIssueGlobal,error,
				Cyclic dependency detected: \

				$(get 1->qualifiedName) ->
				$(with $1,$(map-get includingInstances/$1),
					$(if $(filter $1,$2),,
						$(\s)$(get 2->qualifiedName) ->
						$(call $0,$1,$(map-get includingInstances/$2))))
				$(\s)$(get 1->qualifiedName) -> ... ),),
			Ok))

	# Get ModuleInstance closure of given  Module
	#
	# Args:
	#  1. Module, that not presented in build
	# Return:
	#  List of ModuleInstance for module, that have no reperesents yet
	$(method instanceClosure,
		$(for thisInst<-$1,
			mod <- $(get thisInst->type),
			depMember <- $(get mod->dependsMembers),
			dep <- $(get depMember->modules),
			was <- was$(map-get moduleInstanceStore/$(dep)),
			$(map-set includingInstances/$(mod),$(dep))

			$(if $(invoke chkCyclic,$(dep)),)

			$(for depInst <- $(invoke includeModule,$(dep)),
				$(set* thisInst->depends,$(depInst)))

			$(map-set includingInstances/$(mod),)))

	$(method checkCyclicDependency,
		$(for inst <- $1,
			$(with $(inst),
				$(for \
					inst <-$1,
					depInst <- $(get inst->depends),
					mod <- $(get inst->type),
					depMod <- $(get depInst->type),
					$(if $(map-get includingInstancesChecked/$(depMod)),,
						$(map-set includingInstances/$(mod),$(depMod))

						$(if $(invoke chkCyclic,$(depMod)),
							$(call $0,$(depInst)))

						$(map-set includingInstances/$(mod),)
						$(map-set includingInstancesChecked/$(depMod),1)
						))
				)))

endef

# Takes a resource set of configfiles and creates build for each configuration.
# Returns the argument.
define mybuild_create_build
	$(invoke $(new Mybuild).createBuild,
		$(call mybuild_get_active_configuration,$1))
endef

# 1. Config files resource set.
define mybuild_get_active_configuration
	$(for resource <- $(get 1->resources),
		root <- $(get resource->contentsRoot),
		$(get root->configuration))
endef

# Resolves an instance of ENamedObject typed exported by myfiles model.
#   1. Qualified name.
# Return:
#   Resolved instance, if any.
mybuild_resolve = \
	$(invoke __myfile_resource_set->resolve,$1)

mybuild_resolve_or_die = \
	$(or $(mybuild_resolve), \
		$(error Couldn't resolve Object '$1'))

define printInstance
	$(for inst<-$1,
		mod <- $(get inst->type),
		--- $(get mod->qualifiedName) ---$(\n)
		Inclusion reason: $(if $(get inst->includeMember),
								explicit,
								as dependence)$(\n)
		Depends:$(\n)
		$(for dep <- $(get inst->depends),
			$(\t)$(get $(get dep->type).qualifiedName)$(\n))
		Dependents:$(\n)
		$(for dep <- $(get inst->dependent),
			$(\t)$(get $(get dep->type).qualifiedName)$(\n))
		OptInsts:$(\n)
		$(for optInst <- $(get inst->options),
			opt <- $(get optInst->option),
			val <-$(get optInst->value),
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
						optValue <- $(get $(get annotBind->value).value),
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

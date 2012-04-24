#
#   Date: Feb 10, 2012
# Author: Eldar Abusalimov
#

ifndef __mybuild_mybuild_mk
__mybuild_mybuild_mk := 1

include mk/mybuild/myfile-resource.mk

chekers_list := \
	Mybuild.specifyInstances \
	Mybuild.checkAbstractRealization \
	Mybuild.checkFeatureRealization \
	Mybuild.optionBind


# Main check procedures. Check modules with checkers sequence.
# Out of every checkers passed to next's in, so checkers can add
# or remove instances from input. Every checkers also called with
# passed checkers list, so cheking can be restarted for some
# instances with already passed chekers only.
#
# Args:
# 	1. Cheking instances
# 	2. Checkers list
# 	3. Passed checkers list
define check
	$(if $2,
		$(call check,
			$(call $(firstword $2),$1,$3),
			$(wordlist 2,$(words $2),$2),
			$3 $(firstword $2)),
		$1)
endef

LABEL-DefaultImpl := mybuild.lang.DefaultImpl
LABEL-IfNeed      := mybuild.lang.IfNeed
LABEL-For         := mybuild.lang.For

excludeAnnotations := $(LABEL-IfNeed)
leftToRightParentAnnotations := $(LABEL-IfNeed) $(LABEL-For)

# Constructor args:
#   1. Configuration.
define class-Mybuild
	$(map moduleInstanceStore... : ModuleInstance) #by module
	$(map activeFeatures... : ModuleInstance) #by feature
	$(property-field recommendations)

# Public:

	# Creates build model from the configuration.
	#
	# Args:
	#   1. Configuration.
	# Return:
	#   New instance of CfgBuild.
	$(method createBuild,
		$(assert $(singleword $1))
		$(for configuration <- $1,
			build <-$(new Build),
			issueReceiver <- $(new IssueReceiver),

			$(set build->configuration,$(configuration))
			$(set-field build->issueReceiver,$(issueReceiver))

			$(set build->modules,
				$(invoke getBuildModules))

			$(build))
	)

# Private:

	# Gets all modulesInstance's created according configResourceSet
	#
	# Context:
	#   'configuration'
	#   'issueReceiver'
	# Return:
	#	List of avaible moduleInstances. Some of them may not be created, issue was created
	$(method getBuildModules,
		$(with \
			$(for \
				cfgInclude <- $(get configuration->includes),
				module <- $(get cfgInclude->module),
				$(if $(strip $(call Mybuild.processIncludeAnnotations,$(module),
						$(get cfgInclude->annotations))),

					$(if $(map-get moduleInstanceStore/$(module)),,
						$(invoke moduleInstanceClosure,$(module)))

					$(for moduleInst <- $(invoke moduleInstance,$(module)),
						$(set moduleInst->includeMember,$(cfgInclude))))),
			$(call check,$1,$(chekers_list),)
			))

	$(method processIncludeAnnotations,
		$(if $2,,$1)
		$(for annot <-$2,
			annotName <- $(get annot->qualifiedName),
			$(if $(filter $(annotName),$(excludeAnnotations)),,$1)
			$(if $(filter $(annotName),$(leftToRightParentAnnotations)),
				$(if $(invoke \
						$(get-field \
							$(get $(get annot->bindings).value)
							.value)
						.isSuperTypeOf,$1),,
					$(invoke issueReceiver->addIssues,$(new InstantiateIssue,
						$(get cfgInclude->eResource),
						error,
						$(get cfgInclude->origin),
						Annotation $(annotName) value should be target's parent))))
			$(if $(eq $(annotName),$(LABEL-IfNeed)),
				$(set+ recommendations,
					$(invoke $(get $(get annot->bindings).value).toString)
					$1))))

	$(method specifyInstances,
		$(for inst <- $1,
			$(with $(or $(strip $(for \
						rule<-$(get recommendations),
						targetInstance<-$(map-get build->moduleInstanceByName/$(basename $(rule))),
						targetModule<-$(suffix $(rule)),

						$(if $(and $(filter $(inst),$(targetInstance)),
									$(invoke targetModule->isSubTypeOf,$(get targetInstance->type))),
								$(call check,$(invoke moduleInstanceClosure,$(targetModule)),$2 $0,)
								))),
					$(strip $(for \
						mod <- $(get inst->type),
						annotation <- $(get mod->annotations),
						annotationType <- $(get annotation->type),
#						$(warning 1 $(annotationType))
						$(if $(eq $(get annotationType->qualifiedName),$(LABEL-DefaultImpl)),
#							$(warning $(annotation))
							$(for \
								bind <- $(get annotation->bindings),
								option <- $(get bind->option),
								$(if $(eq $(get option->name),value),
									$(call check,$(invoke moduleInstanceClosure,$(get bind->value)),$2 $0,))))))),
				$(if $1,$1,$(inst)))))

	# Cheker for abstract realization. If there is only one subtype of given abstract module
	# it included to build with all dependencies, which are also checked for abstract
	# realization with `check' restart
	#
	# Args:
	#	1. List of ModuleInstance
	#	2. Checkers, which ModuleInstances was passed.
	$(method checkAbstractRealization,
		$(for inst <- $1,
			instType <- $(get inst->type),
			$(if $(get instType->isAbstract),
				$(if $(singleword $(get instType->subTypes)),
					$(call check,$(invoke moduleInstanceClosure,$(get instType->subTypes)),$2 $0,),
					$(inst)$(invoke issueReceiver->addIssues,$(new InstantiateIssue,,error,,
				   		No abstract realization: $(get instType->qualifiedName)))),
				$(inst))))

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
						$(invoke issueReceiver->addIssues,$(new InstantiateIssue,,error,,
							Feature $(get require->qualifiedName) required by
							$(get instType->qualifiedName) is not implemented))))),
				,$(inst))))



	# Bind options for ModuleInstances (set ModuleInstance optionBinding field with option binding)
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
						$(invoke issueReceiver->addIssues,$(new InstantiateIssue,
							$(for includeMember <- $(get modInst->includeMember),
								$(get includeMember->eResource)),
							warning,
							$(for includeMember <- $(get modInst->includeMember),
								$(get includeMember->origin)),
							Could not bind option $(get opt->name) in module \
								$(get mod->qualifiedName) to a value))Error)))
				,,$(modInst))))

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
			moduleInstance <- $(call Mybuild.moduleInstanceSuper),

			$(invoke setInstanceToType,$(moduleInstance),$(mod))

			$(moduleInstance)))

	# Get ModuleInstance closure of given  Module
	#
	# Args:
	#  1. Module, that not presented in build
	# Return:
	#  List of ModuleInstance for module, that have no reperesents yet
	$(method moduleInstanceClosure,
		$(invoke moduleInstance,$1) \
		$(for thisInst<-$(invoke moduleInstance,$1),
			mod <- $1,
			dep <- $(get mod->depends),
			was <- was$(map-get moduleInstanceStore/$(dep)),
			$(for depInst <- $(invoke moduleInstance,$(dep)),
				$(if $(filter $(depInst),$(get thisInst->depends)),,
					$(set+ thisInst->depends,$(depInst))))
			$(if $(filter was,$(was)),
				$(invoke moduleInstanceClosure,$(dep)))))

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

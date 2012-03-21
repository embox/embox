#
#   Date: Mar 6, 2012
# Author: Anton Kozlov
#

ifndef __mybuild_config_linker_mk
__mybuild_config_linker_mk:= 1

include mk/model/linkage.mk

define class-ConfigLinker
	$(super Linker,$1,$2)

	$(method getResourceImportNormalizers,
		$(for root <- $(get 1->contentsRoot),
			$(with $(get root->name),
				$(if $1,$1.*))) \
		mybuild.lang.*)

	$(method linkHandle-MyFile_OptionBinding_option,
		$(for \
			optBind <- $(invoke 1->eSource),
			parent <- $(invoke optBind->eContainer),

			$(invoke optionBindIn$(class $(parent)),$(parent))))

	$(method optionBindInMyAnnotation,
		$(for annot <- $1,
			annotType <- $(get annot->type),
			opt <- $(get annotType->options),

			$(if $(eq $(get opt->name),$(get link->name)),
				$(opt))))

	$(method optionBindInCfgInclude,
		$(for include <- $1,
			module <- $(get include->module),
			opt <- $(get module->allOptions),
			$(if $(eq $(get opt->name),$(get link->name)),
				$(opt))))
endef

$(def_all)

endif # __mybuild_config_linker_mk

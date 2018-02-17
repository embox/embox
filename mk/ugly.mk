ifndef __mk_ugly_mk
__mk_ugly_mk := 1

#
# Headers.
#

# By header get module
#
define __header_mod_name
	$(strip
		$(subst /,.,
			$(patsubst $(abspath $(SRCGEN_DIR))/include/module/%.h,
					%,
					$(abspath $1))))
endef


module_build_fqn2inst = $(call map-get,$(build_model),moduleInstanceByName,$1)
__header_mod = module_build_fqn2inst

__header_gen = $(with $(__header_mod_name), \
  $(subst $(\n),\n,$(call __header_template,$(call __header_mod,$1),$1)))

# 1. ModuleInstance of most specified module in hierarchy
# 2. ModuleType for which to generate an output
define __module_header_h_template
/* $(GEN_BANNER) */$(\n)
$(for n <- $(subst .,__,$(get 2->qualifiedName)),
		t <- $(get 1->type),
	$(\n)
	$(\h)ifndef __MODULE__$n__H_$(\n)
	$(\h)define __MODULE__$n__H_$(\n)
	$(\n)

	$(if $(eq $(suffix $t),$(suffix $2)),
		// This is the most specific implementation type$(\n)
		$(\h)include <config/$(subst .,/,$(get t->qualifiedName)).h>$(\n)

		$(for impl <- $t $(get t->allSuperTypes),
			$(\n)
			// $(get impl->qualifiedName)$(\n)
			$(or $(for header <- $(call module_get_headers,$(impl)),
				$(\h)include <../../$(header)>$(\n)),
				// (no headers to include)$(\n))),

		// This is a base type of $(get t->qualifiedName)$(\n)
		$(\h)include <module/$(subst .,/,$(get t->qualifiedName)).h>$(\n))

	$(\n)
	$(\h)endif /* __MODULE__$n__H_ */$(\n))
endef

define __inherited_option_id
$(subst \
	$(subst .,__,$(get $(invoke 1->eContainerOfType,
								$(EModel_ENamedObject)).qualifiedName)),
	$(subst .,__,$(get 2->qualifiedName)),
		$(invoke 1->getId))
endef

# 1. ModuleInstance of most specified module in hierarchy
# 2. ModuleType for which to generate an output
define __module_config_h_template
/* $(GEN_BANNER) */$(\n)
$(for n <- $(subst .,__,$(get 2->qualifiedName)),
		t <- $(get 1->type),
	$(\n)
	$(\h)ifndef __CONFIG__$n__H_$(\n)
	$(\h)define __CONFIG__$n__H_$(\n)

	$(if $(eq $(suffix $t),$(suffix $2)),
		// This is the most specific implementation type$(\n)

		$(with $(strip $(get 1->options)),$2,
			$(if $1,$(for op <- $1,
				optionId <- $(call __inherited_option_id,$(get op->option),$t),
				$(\n)
				$(\h)ifndef $(optionId)$(\n)
				$(\h)define $(optionId) $(invoke op->value>toString)$(\n)
				$(\h)endif$(\n)),
				// (no config options)$(\n)))

		$(for 2 <- $(filter-out %$(suffix $t),$(get t->allSuperTypes)),
			$(with $(strip $(get 1->options)),
					$(strip $(get 2->allOptions)),
				$(if $2,$(for superType <- $2,
					superName <- $(get superType->name),
					superId <- $(invoke superType->getId),
					optionId <- $(call __inherited_option_id,$(superType),$t),
					$(\n)
					$(\h)ifndef $(superId)$(\n)
					$(\h)define $(superId) \$(\n)
					$(\t)$(\t)$(optionId)$(\n)
					$(\h)endif$(\n))))),

		// This is a base type of $(get t->qualifiedName)$(\n)
		$(\h)include <config/$(subst .,/,$(get t->qualifiedName)).h>$(\n))

	$(\n)$(\h)endif /* __CONFIG__$n__H_ */$(\n))
endef

module_get_headers = \
	$(patsubst $(abspath $(ROOT_DIR))/%,%, \
		$(abspath $(filter %.h,$(module_get_files))))
module_get_files = \
	$(foreach s,$(get 1->sources),$(get s->fileFullName))

filter_static_reacheables=$(get $1.depends)
define filter_static_modules
	$(call topsort,$(strip $(foreach m,$1,$(if $(get $(get m->type).isStatic),$m))),filter_static_reacheables)
endef

_topsort_error_fatal=
# TODO enable fatal mode
#_topsort_error_fatal=1

# Performs topological sort of library modules.
# 1. Vertexes
# 2. Function of one argument returning vertex's reacheable vertexes
define topsort
	$(shell $(if $(_topsort_error_fatal),set -eo pipefail;) echo $(foreach v,$1,$(foreach u,$(filter $1,$(call $2,$v)) $v,$v $u)) | $(TSORT) 2>&1 | awk -f $(ROOT_DIR)/mk/tsortdiag.awk | $(TAC))
	$(if $(_topsort_error_fatal),$(if $(eq 0,$(.SHELLSTATUS)),,$(info Error: stop after finding cycle(s))$(error Mybuild stop)))
endef


$(def_all)

endif # __mk_ugly_mk

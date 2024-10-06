#
#   Date: May 12, 2021
# Author: Alexander Kalmuk
#

include mk/script/script-common.mk
include mk/script/build-deps.mk

my_app := $(call mybuild_resolve_or_die,mybuild.lang.App)

is_app = \
	$(strip $(call invoke, \
		$(call get,$1,allTypes),getAnnotationsOfType,$(my_app)))

fqn2id = $(subst .,__,$1)

apps := \
	$(foreach m,$(call get,$(build_model),modules), \
		$(if $(call is_app,$m), \
			$(call get,$m,type)))

define __app_build_deps_array_start
$3 __module_$(strip $1)_build_deps_$2_$4[] = {
endef

define __app_build_deps_array_elem
	($3)&__module_$(strip $1)_$2_$4,
endef

define __app_build_deps_array_end
	$1
};

endef

define __app_build_deps_extern_ref
extern char __module_$(strip $1)_$2_$3;
endef

app_build_deps_extern_ref = \
	$(foreach d,$(call build_deps_all,$1), \
		$(info $(call __app_build_deps_extern_ref, \
			$(call fqn2id,$(call get,$d,qualifiedName)),$2,$3) \
		) \
	)

app_build_deps_array_start = \
	$(call __app_build_deps_array_start, \
		$(call fqn2id,$(call get,$1,qualifiedName)),$2,$3,$4)

app_build_deps_array_body = \
	$(foreach d,$(call build_deps_all,$1), \
		$(info $(call __app_build_deps_array_elem, \
			$(call fqn2id,$(call get,$d,qualifiedName)),$2,$3,$4) \
		) \
	)

app_build_deps_array_end = \
	$(call __app_build_deps_array_end,$1)

$(foreach s,data bss, \
	$(foreach m,$(apps), \
		$(info $(call app_build_deps_extern_ref,$m,$s,vma)) \
		$(info $(call app_build_deps_array_start,$m,$s,void *,vma)) \
		$(info $(call app_build_deps_array_body,$m,$s,void *,vma)) \
		$(info $(call app_build_deps_array_end,(void *)0,vma)) \
	) \
)

$(foreach s,data, \
	$(foreach m,$(apps), \
		$(info $(call app_build_deps_extern_ref,$m,$s,lma)) \
		$(info $(call app_build_deps_array_start,$m,$s,void *,lma)) \
		$(info $(call app_build_deps_array_body,$m,$s,void *,lma)) \
		$(info $(call app_build_deps_array_end,(void *)0,lma)) \
	) \
)

$(foreach s,data bss, \
	$(foreach m,$(apps), \
		$(info $(call app_build_deps_extern_ref,$m,$s,len)) \
		$(info $(call app_build_deps_array_start,$m,$s,unsigned long,len)) \
		$(info $(call app_build_deps_array_body,$m,$s,unsigned long,len)) \
		$(info $(call app_build_deps_array_end,(unsigned long)-1,len)) \
	) \
)

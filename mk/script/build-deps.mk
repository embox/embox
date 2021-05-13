#
# Returns list of modules connected with @BuildDepends().
#
#   Date: May 12, 2021
# Author: Alexander Kalmuk
#

# 1. Annotation target
# 2. Annotation option
__annotation_value = $(call get,$(call invoke,$1,getAnnotationValuesOfOption,$2),value)

__my_bld_dep_value := $(call mybuild_resolve_or_die,mybuild.lang.BuildDepends.value)

build_deps = $(call __annotation_value,$1,$(__my_bld_dep_value))

# Maps moduleType to that one, which instance is in build. For example, if 'api' extended
# by 'impl1' and 'impl2', and 'impl2' is in build, calling this with 'api' moduleType will
# return moduleType of 'impl2'.
# 1. moduleType
__build_module_type_substitude = \
	$(call get,$(call module_build_fqn2inst,$(call get,$1,qualifiedName)),type)

__build_deps_recurse = $1 $(call build_deps_all,$1)

build_deps_all = \
	$(sort $(foreach d,$(call build_deps,$1), \
		$(call __build_deps_recurse,$(call __build_module_type_substitude,$d))))

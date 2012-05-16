
generate_header := \
  /* Auto-generated EMBuild Dependency Injection model file. Do not edit. */

generate_includes := \#include <framework/mod/embuild.h>$(\n)

$(info $(generate_header))
$(info )
$(info $(generate_includes))
$(info $(generate_package_defs))
$(info $(generate_mod_defs))
$(info $(generate_mod_deps))

.PHONY : all

all :
	@#

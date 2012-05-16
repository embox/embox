#
# Bootstrap script.
#
# It, first, ensures that all cached scripts are up-to-date, regenerating them
# if necessary. Next, it searches the source tree for my- and config-files,
# parses and links them together, and saves the resulting object graph.
#
# When everything is ready, the script spawns a sub-make for 'mk/build.mk',
# preloading all the generated scripts and exporting the following variables:
#
#   load_mk_files
#     List of files, where the cached scripts (Make functions) have been saved.
#
#   load_mybuild_files
#     Lists the following three files (see below):
#        '$(myfiles_model_mk) $(configfiles_model_mk) $(build_model_mk)'
#
#   myfiles_model_mk
#     As the name proposes, it is a file where all my-files linked together
#     are stored.
#
#   configfiles_model_mk
#     Config-files linked together and with my-files.
#
#   build_model_mk
#     Build model inferred from the two models described above.
#
#   Date: Apr 3, 2012
# Author: Eldar Abusalimov
#

.DELETE_ON_ERROR:
.SECONDEXPANSION:

# The order of these inclusions is important.
include mk/load-mk.inc.mk
include mk/load-mybuild.inc.mk

.PHONY : $(MAKECMDGOALS)
$(MAKECMDGOALS) : $(load_mk_files) $(load_mybuild_files)
	@$(MAKE) -f mk/build.mk MAKEFILES='$^' $@

%/. :
	@$(MKDIR) $*

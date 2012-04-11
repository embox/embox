#
# Bootstrap script.
#
#   Date: Apr 3, 2012
# Author: Eldar Abusalimov
#

.DELETE_ON_ERROR:
.SECONDEXPANSION:

include mk/load-mk.inc.mk
include mk/load-mybuild.inc.mk

.PHONY : $(MAKECMDGOALS)
$(MAKECMDGOALS) : $(load_mk_files) $(load_mybuild_files)
	@$(MAKE) -f mk/build.mk MAKEFILES='$^' $@

%/. :
	@$(MKDIR) $*

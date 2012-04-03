#
# Bootstrap script.
#
#   Date: Apr 3, 2012
# Author: Eldar Abusalimov
#

CACHE_DIR := mk/.cache

.DELETE_ON_ERROR:
.SECONDEXPANSION:

include mk/load-mk.inc.mk
include mk/load-mybuild.inc.mk

$(MAKECMDGOALS) : $(load_mk_files) $(load_mybuild_files)
	@$(MAKE) -f mk/image.mk MAKEFILES='$^' $@


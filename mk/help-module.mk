#
#   Date: Apr 5, 2012
# Author: Anton Kozlov
#

help_modulelist = \
	$(foreach inst,$(call get,$(build_model),modules),\
		$(call get,$(call get,$(inst),type),qualifiedName))

help_module = \
	$(call printInstance,$(call map-get,$(build_model),moduleInstanceByName,$1))

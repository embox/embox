#
#   Date: Feb 10, 2012
# Author: Eldar Abusalimov
#

ifndef __mybuild_mybuild_mk
__mybuild_mybuild_mk := 1

include mk/mybuild/myfile-resource.mk

# Constructor args:
#   1. Myfile resource set
#   1. Config resource set
define class-Mybuild
	$(property-field myfileSet : ResourceSet,$1)
	$(property-field configSet : ResourceSet,$2)

endef

mybuild_model_instance = $(__mybuild_model_instance)

$(def_all)

endif # __mybuild_mybuild_mk

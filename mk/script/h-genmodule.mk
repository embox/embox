#
#   Date: Apr 12, 2012
# Author: Eldar Abusalimov
#

include mk/script/script-common.mk

ifndef GENMODULE_INSTANCE
$(error GENMODULE_INSTANCE is not defined, nothing to output)
endif

override GENMODULE_INSTANCE := $(GENMODULE_INSTANCE)

# I prefer lower-case identifiers...
__genmodule_instance := $(GENMODULE_INSTANCE)
__genmodule_type     := $(call get,$(__genmodule_instance),type)
__genmodule_name     := $(call get,$(__genmodule_type),qualifiedName)

__genmodule_escaped_name := $(subst .,__,$(__genmodule_name))
__genmodule_inclusion_guard := __GENMODULE__$(__genmodule_escaped_name)__H_

$(info /* $(GEN_BANNER) */)
$(info )
$(info #ifndef $(__genmodule_inclusion_guard))
$(info #define $(__genmodule_inclusion_guard))
$(info )
$(info )
$(info #endif /* $(__genmodule_inclusion_guard) */)

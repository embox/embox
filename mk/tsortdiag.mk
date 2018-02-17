
include mk/script/script-common.mk

$(foreach e,$E,\
	$(if $(findstring newcycle,$e),\
		$(info Error: modules dependency cycle:),\
		$(info $(\t)$(call get,$(call get,$e,type),qualifiedName))))

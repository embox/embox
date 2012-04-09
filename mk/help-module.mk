#
#   Date: Apr 5, 2012
# Author: Anton Kozlov
#

__inst_name = $(call get,$(call get,$1,type),qualifiedName)

__name_inst = $(call map-get,$(build_model),moduleInstanceByName,$1)

mod_list = \
	$(foreach inst,$(call get,$(build_model),modules),\
		$(call __inst_name,$(inst)))

mod_brief = \
	$(call printInstance,$(__name_inst))

mod_include_reason = $(call __mod_include_reason,$(__name_inst))

__mod_include_reason = \
	 $(value 2)$(__inst_name): \
	 $(if $(call get,$1,includeMember),\
		explicit$(\n),\
		as dependence:$(\n)\
		$(foreach d,$(strip $(call get,$1,dependent)),\
			$(call __mod_include_reason,$d,$(value 2)$(\t))))\

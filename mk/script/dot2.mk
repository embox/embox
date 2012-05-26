#
#   Date: Apr 3, 2012
# Author: Eldar Abusalimov
#

include mk/script/script-common.mk

ifndef __myfile_resource_set
$(error __myfile_resource_set is not defined)
endif

module_types := \
	$(strip $(foreach e,$(call get,$(__myfile_resource_set),contents), \
		$(if $(call invoke,$(MyFile_ModuleType),isInstance,$e),$e)))

module_types := \
	$(sort $(foreach m,$(call get,$(build_model),modules),$(call get,$m,allTypes)))

define options
	graph [
		rankdir = "LR"
		compound = true
	];
	node [
		fontsize = "16"
		shape = "box"
		style = "rounded,filled"
		penwidth = "2"
	];
	edge [
		penwidth = "3"
	];
  overlap=false;
endef
#  ratio=compress; $(\n)\
#  size="50,50"; $(\n)\
#  concentrate=true; $(\n)\
#  ranksep="1.0 equal"; $(\n)\
#  K=1.0; $(\n)\

$(info digraph Embox {)
$(info $(options))
$(foreach t,$(module_types), \
	$(foreach fqn,$(call get,$t,qualifiedName), \
		$(foreach pkg,$(or $(basename $(fqn)),[default]), \
	$(info $(\t)"$t" [label= \
		<<TABLE BORDER="0" CELLBORDER="0" CELLSPACING="0" WIDTH="150"> \
  			<TR><TD WIDTH="150" ALIGN="LEFT"><FONT POINT-SIZE="10.0">$(pkg)</FONT></TD></TR> \
  			<TR><TD WIDTH="150" ALIGN="RIGHT"><FONT POINT-SIZE="16.0">$(call get,$t,name)</FONT></TD></TR> \
		</TABLE>>, \
		color="$(\h)$(if $(call get,$t,isAbstract),93ee93,b2c6df)", \
		fillcolor="$(\h)$(if $(call get,$t,isAbstract),dcfadc,e7eef5)"];) \
	$(foreach s,$(call get,$t,superType), \
		$(info $(\t)"$t" -> "$s" [color="$(\h)b3b3ff", \
			headport=s,tailport=n];)) \
	$(foreach d,$(call get,$t,depends), \
		$(info $(\t)"$t" -> "$d" [color="$(\h)ffb3b3"];)))))
$(info })
#		$(info $(\t){ rank="source"; "$t"; "$s"; }) \


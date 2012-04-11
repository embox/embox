#
#   Date: Apr 3, 2012
# Author: Anton Kozlov
#

include mk/script/script-common.mk

ifndef BUILD_MODEL
$(error BUILD_MODEL is not defined)
endif

options = \
  ratio=compress; $(\n)\
  size="50,50"; $(\n)\
  concentrate=true; $(\n)\
  ranksep="1.0 equal"; $(\n)\
  K=1.0; $(\n)\
  overlap=false;

header := digraph Embox {

footer := }

mod_package = $(basename $(mod_name))
mod_name = $(call get_inst_name,$(mod))

get_modules = \
	$(foreach mod,$(call get,$1,modules),$(call get,$(mod),type))

get_inst_name = \
	$(call get,$(call get,$1,type),qualifiedName)

$(info $(header))

$(info $(options))

$(info )
$(foreach package,\
	$(sort $(basename $(foreach mod,$(call get_modules,$(BUILD_MODEL)),\
			$(call get,$(mod),qualifiedName)))),\
 $(info subgraph "cluster.$(package)" {)\
 $(info    node [style=filled,fillcolor=white];)\
 $(info    graph [label = "$(package)",style=rounded,style=filled,color=lightgray];)\
    $(foreach mod,$(call get,$(BUILD_MODEL),modules),\
    		$(if $(filter $(package),$(mod_package)),\
			$(info "$(mod)" [label = "$(mod_name)"];)))\
			$(info }))

$(foreach mod,$(call get,$(BUILD_MODEL),modules),\
  $(foreach dep,$(call get,$(mod),depends),\
    $(info "$(mod)" -> "$(dep)";)))

$(info $(footer))


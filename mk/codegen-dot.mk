#
# Dot tool binding.
# NOTE: This code is not assumed to be good. Just to see how does it work.
#
# Author: Eldar Abusalimov
#

ifndef _codegen_dot_mk_
_codegen_dot_mk_ := 1

#include $(MK_DIR)/embuild.mk

GRAPH = $(MODS_BUILD)
GRAPH_DOT = $(CODEGEN_DIR)/mod_dag.dot
GRAPH_PS  = $(DOT_DIR)/$(TARGET).ps

mod_package = $(basename $(mod))
mod_name = $(patsubst .%,%,$(suffix $(mod)))

options = \
  ratio=compress; \
  size="50,50"; \
  concentrate=true; \
  ranksep="1.0 equal"; \
  K=1.0; \
  overlap=false;

generate_dot = $(strip \ndigraph Embox { \
  $(options)\
  $(foreach package,$(sort $(basename $(GRAPH))), \
    \nsubgraph "cluster.$(package)" { \
      \nnode [style=filled,fillcolor=white]; \
      \ngraph [label = "$(package)",style=rounded,style=filled,color=lightgray]; \
      $(foreach mod,$(GRAPH),$(if $(filter $(package),$(mod_package)),\
        \n"$(mod)" [label = "$(mod_name)"];\
      )) \
    \n} \
  ) \
  $(foreach mod,$(GRAPH),\
    $(foreach dep,$(DEPS-$(mod)), \
      \n"$(mod)" -> "$(dep)"; \
    ) \
  ) \
\n})\n

$(GRAPH_DOT) : $(MK_DIR)/codegen-dot.mk
	@$(PRINTF) '$(generate_dot)' > $@

$(GRAPH_PS) : $(GRAPH_DOT)
	mkdir -p $(DOT_DIR) && fdp -Tps $< -o $@

endif

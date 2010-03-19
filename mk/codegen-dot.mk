#
# Dot tool binding.
# NOTE: This code is not assumed to be good. Just to see how does it work.
#
# Author: Eldar Abusalimov
#

ifndef _codegen_dot_mk_
_codegen_dot_mk_ := 1

include $(MK_DIR)/embuild.mk

GRAPH_DOT = $(CODEGEN_DIR)/mod_dag.dot
GRAPH_PS  = $(DOT_DIR)/$(TARGET).ps

mod_package = $(basename $(mod))
mod_name = $(patsubst .%,%,$(suffix $(mod)))

generate_dot = $(strip \ndigraph EMBOX { \
  ratio=compress; size="50,50"; concentrate=true; ranksep="1.0 equal";\
  $(foreach package,$(sort $(basename $(MODS_BUILD))), \
    \nsubgraph "cluster.$(package)" { \
      \nnode [style=filled,fillcolor=white]; \
      \ngraph [label = "$(package)",style=rounded,style=filled,color=lightgray]; \
      $(foreach mod,$(MODS_BUILD),$(if $(filter $(package),$(mod_package)),\
        \n"$(mod)" [label = "$(mod_name)"];\
      )) \
    \n} \
  ) \
  $(foreach mod,$(MODS_BUILD),\
    $(foreach dep,$(DEPS-$(mod)), \
      \n"$(mod)" -> "$(dep)"; \
    ) \
  ) \
\n})\n

$(GRAPH_DOT) : $(EMBUILD_DUMP_PREREQUISITES) $(MK_DIR)/codegen-dot.mk
	@$(PRINTF) '$(generate_dot)' > $@

$(GRAPH_PS) : $(GRAPH_DOT)
	@mkdir -p $(DOT_DIR) && dot -Tps $< -o $@

endif

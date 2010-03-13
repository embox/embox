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

dot_mod = $(call dot_escape,$(mod))
dot_dep = $(call dot_escape,$(dep))

dot_escape = $(subst .,_,$(1))

generate_dot = $(strip \ndigraph EMBOX { \
  $(foreach mod,$(MODS_BUILD), \
    $(foreach dep,$(DEPS-$(mod)), \
      \n$(dot_mod) -> $(dot_dep); \
    ) \
  ) \
\n})

$(GRAPH_DOT) : $(EMBUILD_DUMP_PREREQUISITES) $(MK_DIR)/codegen-dot.mk
	@$(PRINTF) '$(generate_dot)' > $@

$(GRAPH_PS) : $(GRAPH_DOT)
	@mkdir -p $(DOT_DIR) && dot -Tps $< -o $@

endif

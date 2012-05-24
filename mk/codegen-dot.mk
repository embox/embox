#
# Dot tool binding.
# NOTE: This code is not assumed to be good. Just to see how does it work.
#
# Author: Eldar Abusalimov
#

ifndef _codegen_dot_mk_
_codegen_dot_mk_ := 1

#include mk/embuild.mk

GRAPH_DOT = $(SRCGEN_DIR)/mod_dag.dot
GRAPH_PS  = $(DOT_DIR)/$(TARGET).ps

dot: $(GRAPH_PS)
	@echo 'Dot complete'

$(GRAPH_DOT) : mk/codegen-dot.mk
$(GRAPH_DOT) : mk/script/dot2.mk
	@$(MKDIR) $(SRCGEN_DIR) && $(MAKE) -f $< BUILD_MODEL=$(build_model) > $@

$(GRAPH_PS) : $(GRAPH_DOT)
	@$(MKDIR) $(DOT_DIR) && dot -Tps $< -o $@

endif

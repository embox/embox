#
# Common rules of compiling the EMBOX sources.
#
# Author: Eldar Abusalimov
#

CCMAKEDEPS:=-MMD# -MT $@ -MF $(@:.o=.d)

$(OBJ_DIR)/%.o::$(SRC_DIR)/%.c
	$(CC) $(CCFLAGS) $(CCINCLUDES) $(CCMAKEDEPS) -c -o $@ $<

$(OBJ_DIR)/%.o::$(SRC_DIR)/%.S
	$(CC) $(CCFLAGS) $(CCINCLUDES) $(CCMAKEDEPS) -c -o $@ $<

# Disable some builtin rules. These speeds up the build a bit.
.SUFFIXES:
% : RCS/%
% : %,v
% : RCS/%,v
% : %,w
% : RCS/%,w
% : s.%
% : SCCS/s.%
% : %.w

%.c : %.y
%.c : %.l
%.c : %.w

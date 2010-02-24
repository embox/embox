#
# Author: Eldar Abusalimov
#

.DELETE_ON_ERROR:

# Disable some annoying builtin rules. These speeds up the build a bit.
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

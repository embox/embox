#
# Converts the output of 'nm' into a C source containing an array of symbols
# that correspond to functions (t and T in terms of 'nm').
#
# Usage:
#   nm -n image | awk -f nm2c.awk
#
#   Date: Aug 28, 2012
# Author: Eldar Abusalimov
#


BEGIN {
	print "/* Auto-generated file. Do not edit. */";
	print "";
	print "#include <debug/symbol.h>";
	print "#include <stddef.h>";
	print "";
	print "const struct symbol __symbol_table[] = {";
}

/^[0-9a-fA-F]* [tT]/ {
	split($4,a,":");
	if (a[1]) {
		sub(/\.\/\.\//, "./", a[1])
		loc = sprintf("{ \"%s\", %d }", a[1], a[2])
	} else {
		loc = "{ 0 }"
	}
	printf "\t{ (void *) 0x%s, \"%s\", %s },\n", $1, $3, loc;
}

END {
	print "};";
	print "const size_t __symbol_table_size =";
	print "\tsizeof(__symbol_table) / sizeof(__symbol_table[0]);";
	print "";
}

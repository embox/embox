# don't care of anything not labeled by "tsort:" diag output
$1 != "tsort:" {
	print
	next
}

# Two options
# "tsort: -: ..."
# "tsort: .bld502x"
{
	e = e " " ($2 == "-:" ?  "newcycle" : $2)
}

END {
	system("E=\"" e "\" make -f ./mk/tsortdiag.mk 1>&2")
}

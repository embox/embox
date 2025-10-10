
cppflags="$EMBOX_DEPS_CPPFLAGS_BEFORE $EMBOX_IMPORTED_CPPFLAGS $EMBOX_DEPS_CPPFLAGS_AFTER"

cmd=$(basename $0)
cmd=${cmd#embox-}

case $cmd in
	gcc|clang)   arg_line_before="$EMBOX_IMPORTED_CFLAGS";;
	g++|clang++) arg_line_before="$EMBOX_IMPORTED_CXXFLAGS";;
esac

case $EMBOX_GCC_LINK in
	full) arg_line_after="$cppflags $EMBOX_IMPORTED_LDFLAGS $EMBOX_IMPORTED_LDFLAGS_FULL";;
	*)    arg_line_after="$cppflags $EMBOX_IMPORTED_LDFLAGS -Wl,-r";;
esac

case " $@ " in
	*" -c "*) arg_line_after="$cppflags";;
	*" -E "*) arg_line_after="$cppflags"; arg_line_before="$EMBOX_IMPORTED_M_CFLAGS";;
esac

# Command line arguments ("$@") follow imported cflags and cxxflags and can override them.
# Imported cppflags follow command line arguments ("$@") for '#include_next' to work.
# Imported ldflags must follow input files from command line arguments ("$@").
${EMBOX_CROSS_COMPILE}${cmd} $arg_line_before "$@" $arg_line_after
exit $?

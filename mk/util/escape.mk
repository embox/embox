ifndef __util_escape_mk
__util_escape_mk := 1

# The function escapes following symbols:
# $(\n), $$ and insert $(\0) at the end and start of the line
define escape_makefile
	$(or \
		$$(\0)
		$(subst $(\n),$$(\n),
			$(subst $$,$$$$,$1)
		)
		$$(\0),
		$(error Empty input)
	)
endef

define escape_printf
	$(or \
		$(subst $(\n),\n,
			$(subst \,\\,$1)
		),
		$(error Empty input)
	)
endef

$(def_all)

endif #__util_escape_mk

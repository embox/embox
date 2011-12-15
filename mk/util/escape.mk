# The function escapes following symbols:
# \n, $ and '\' at the end of the line
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

$(def_all)

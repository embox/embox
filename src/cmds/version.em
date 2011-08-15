$_MODS += version
$_SRCS-version += version.c
$_BRIEF-version = Displays info associated with compilation

define $_DETAILS-version
	NAME
		version - displays info associated with compilation
	AUTHORS
		Nikolay Korotky
endef


#TODO this must be a script or special make target

¹SVN_REV = $(shell svn info $(ROOT_DIR) | grep Rev: | awk '{print $$4}')

svn_get_revision:
ifeq ($(SVN_REV),)
	@echo "svn cmd not found"
else
	#to $(config_h)
	@echo "#define CONFIG_SVN_REV $(SVN_REV)" >> $@
endif

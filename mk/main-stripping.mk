ifeq ($(TARGET_APP),)
$(error TARGET_APP is not set.)
endif

ifeq ($(FILE_APP),)
$(error FILE_APP is not set.)
endif

$(FILE_APP) : $(subst strip.,,$(FILE_APP))
	$(ROOT_DIR)/mk/main-stripping.sh $(TARGET_APP) $< $@

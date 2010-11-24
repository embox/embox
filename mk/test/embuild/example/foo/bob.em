
define module bob
  sources = *.c
  $(error should not be expanded by emfile sandbox)
endef

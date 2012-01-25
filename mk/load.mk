
mk_core_cache := mk/cache/mk_core.mk

mk_core_scripts := \
	$(addprefix mk/core/,common.mk string.mk define.mk object.mk)

all : $(mk_core_cache)

$(mk_core_cache) : $(mk_core_scripts)
	$(MAKE) -f mk/cache.mk CACHE_INCLUDES='$^' > $@

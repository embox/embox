
.PHONY: all $(CPU_ARCH)

create_objs_lst:$(CPU_ARCH)
	@echo cpu_arch = $(CPU_ARCH)

$(CPU_ARCH):
	$(MAKE) -C $@ $(MAKEOP)


all:$(CPU_ARCH)



clean:$(CPU_ARCH)

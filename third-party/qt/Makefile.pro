QMAKE := $(ROOT_DIR)/build/qt/install/bin/qmake

.PHONY: %
%:
	mkdir -p build/base/obj/$(BUILD_DIR) && \
		cd build/base/obj/$(BUILD_DIR) && \
		$(QMAKE) '$(ROOT_DIR)/$(@:.%=%)/$(notdir $@).pro' && \
		cp '$(ROOT_DIR)/build/qt/build/.qmake.cache' . && \
		make
#MAKEFLAGS='-j $(shell nproc))'

.NOTPARALLEL:

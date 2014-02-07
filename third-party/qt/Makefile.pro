QMAKE := $(ROOT_DIR)/build/qt/install/bin/qmake

.PHONY: %
%:
	mkdir -p build/base/obj/$(BUILD_DIR) && \
		cd build/base/obj/$(BUILD_DIR) && \
		cp '$(ROOT_DIR)/build/qt/build/.qmake.cache' . && \
		$(QMAKE) '$(ROOT_DIR)/$(@:.%=%)/$(notdir $@).pro' && \
		make MAKEFLAGS='-j $(shell nproc))'

.NOTPARALLEL:

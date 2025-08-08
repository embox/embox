PKG_NAME          := tflite-micro
PKG_VER           := main
PKG_REPO          := https://github.com/tensorflow/$(PKG_NAME).git
PKG_SOURCE_DIR    := $(PKG_NAME)
PKG_BUILD_DIR     := build/$(PKG_NAME)

TFLM_FLATBUFFERS_VER := 23.5.26
TFLM_GEMMLOWP_VER     := 719139ce755a0f31cbf1c37f7f98adcc7fc9f425
TFLM_RUY_VER          := d37128311b445e758136b8602d1bbd2a755e115d
TFLM_KISSFFT_VER      := v130


FLATBUFFERS_URL := https://github.com/google/flatbuffers/archive/v$(TFLM_FLATBUFFERS_VER).zip
GEMMLOWP_URL    := https://github.com/google/gemmlowp/archive/$(TFLM_GEMMLOWP_VER).zip
RUY_URL         := https://github.com/google/ruy/archive/$(TFLM_RUY_VER).zip
KISSFFT_URL     := https://github.com/mborgerding/kissfft/archive/refs/tags/$(TFLM_KISSFFT_VER).zip

MD5_FLATBUFFERS := e87e8acd8e2d53653387ad78720316e2
MD5_GEMMLOWP    := 7e8191b24853d75de2af87622ad293ba
MD5_RUY         := abf7a91eb90d195f016ebe0be885bb6e
MD5_KISSFFT     := 438ba1fef5783cc5f5f201395cc477ca


CXX := g++
CXXFLAGS := -std=c++17 -O3 -Wall -Wno-narrowing


INCLUDE_DIRS := \
  -I$(PKG_SOURCE_DIR) \
  -Ithird_party_static/flatbuffers/include \
  -Ithird_party_static/gemmlowp \
  -Ithird_party_static/kissfft \
  -Ithird_party_static/ruy

THIRD_PARTY := third_party_static

.PHONY: all clone third_party configure build install clean


CONFIGURE := .configured
BUILD     := .built
INSTALL   := .installed

all: install

clone:
	@if [ ! -d "$(PKG_SOURCE_DIR)" ]; then \
		echo "Cloning TensorFlow Lite Micro..."; \
		git clone --depth=1 -b $(PKG_VER) $(PKG_REPO) $(PKG_SOURCE_DIR); \
	else \
		echo "Already cloned."; \
	fi


define download_zip
	@echo "Downloading: $(1)"
	@mkdir -p temp && cd temp && \
	curl -L -o archive.zip $(1) && \
	echo "$(2)  archive.zip" | md5sum -c - && \
	unzip -q archive.zip && \
	mkdir -p "$(abspath $(3))" && \
	mv */* "$(abspath $(3))" && \
	cd ../.. && rm -rf temp
endef


third_party: clone
	mkdir -p $(THIRD_PARTY)
	$(call download_zip,$(GEMMLOWP_URL),$(MD5_GEMMLOWP),$(THIRD_PARTY)/gemmlowp)
	$(call download_zip,$(RUY_URL),$(MD5_RUY),$(THIRD_PARTY)/ruy)
	$(call download_zip,$(KISSFFT_URL),$(MD5_KISSFFT),$(THIRD_PARTY)/kissfft)
	$(call download_zip,$(FLATBUFFERS_URL),$(MD5_FLATBUFFERS),$(THIRD_PARTY)/flatbuffers)


THIRD_PARTY_SRCS := \
  $(wildcard $(THIRD_PARTY)/ruy/*.cc) \
  $(wildcard $(THIRD_PARTY)/gemmlowp/*.cc) \
  $(wildcard $(THIRD_PARTY)/flatbuffers/*.c) \
  $(wildcard $(THIRD_PARTY)/kissfft/kiss_fft.c) \
  $(wildcard $(THIRD_PARTY)/kissfft/kiss_fftr.c) 

SRCS := \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/lite/micro             -maxdepth 1 -name '*.cc' ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/lite/micro/arena_allocator -name '*.cc' ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/lite/micro/memory_planner -name '*.cc' ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/lite/micro/tflite_bridge -name '*.cc' ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/lite/micro/kernels      -maxdepth 1 -name '*.cc' ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/lite/core               -maxdepth 1 -name '*.cc' ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/lite/core/api           -name '*.cc' ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/lite/core/c             -name '*.cc' ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/lite/kernels/internal   -name '*.cc' ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/lite/kernels            -maxdepth 1 -name '*.cc' ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/lite/c                  -name '*.c'  ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/lite                    -maxdepth 1 -name '*.c'  ! -name '*test.cc') \
  $(PKG_SOURCE_DIR)/tensorflow/lite/array.cc \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/lite/schema             -name '*.cc' ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/compiler/mlir/lite/core/api   -name '*.cc' ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/tensorflow/compiler/mlir/lite/schema     -name '*.cc' ! -name '*test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/signal/micro -name '*.cc' ! -name 'test.cc') \
  $(shell find $(PKG_SOURCE_DIR)/signal/src -name '*.cc' ! -name 'test.cc') \
  $(THIRD_PARTY_SRCS)

OBJS := $(patsubst $(PKG_SOURCE_DIR)/%.cc, $(PKG_BUILD_DIR)/%.o, $(filter %.cc,$(SRCS))) \
        $(patsubst $(PKG_SOURCE_DIR)/%.c,  $(PKG_BUILD_DIR)/%.o, $(filter %.c,$(SRCS)))

$(PKG_BUILD_DIR)/libtflm.a: $(OBJS)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(PKG_BUILD_DIR)/%.o: $(PKG_SOURCE_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c "$<" -o "$@"

$(PKG_BUILD_DIR)/%.o: $(PKG_SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -x c -c "$<" -o "$@"


$(CONFIGURE):
	$(MAKE) clone
	$(MAKE) third_party
	touch $@

$(BUILD): $(CONFIGURE)
	$(MAKE) $(PKG_BUILD_DIR)/libtflm.a
	touch $@

$(INSTALL): $(BUILD)
	@echo "Installing libtflm.a and headers..."


	@mkdir -p install/lib install/include
	@cp $(PKG_BUILD_DIR)/libtflm.a install/lib/


	@mkdir -p $(EXTERNAL_BUILD_DIR)/third_party/lib/tensorflow/build/tflite-micro
	@cp $(PKG_BUILD_DIR)/libtflm.a $(EXTERNAL_BUILD_DIR)/third_party/lib/tensorflow/build/tflite-micro


	@find $(PKG_SOURCE_DIR)/tensorflow/lite -name '*.h' -exec cp --parents {} install/include/ \;

	touch $@


configure: $(CONFIGURE)
build:     $(BUILD)
install:   $(INSTALL)


clean:
	rm -rf $(PKG_SOURCE_DIR) $(PKG_BUILD_DIR) $(THIRD_PARTY) install .configured .built .installed

CXX ?= g++
CC ?= gcc

TFLM_BUILD_DIR ?= ../../../third-party/lib/tensorflow/build/tflite-micro
TFLM_ROOT      ?= ../../../third-party/lib/tensorflow/tflite-micro
TPARTY_DIR     ?= ../../../third-party/lib/tensorflow/third_party_static

INCLUDES = \
  -I$(TFLM_ROOT) \
  -I$(TFLM_ROOT)/tensorflow \
  -I$(TFLM_ROOT)/tensorflow/lite \
  -I$(TFLM_ROOT)/tensorflow/lite/core/api \
  -I$(TFLM_ROOT)/tensorflow/lite/core/c \
  -I$(TFLM_ROOT)/tensorflow/lite/micro \
  -I$(TFLM_ROOT)/tensorflow/lite/schema \
  -I$(TPARTY_DIR)/flatbuffers/include \
  -I$(TPARTY_DIR)/gemmlowp \
  -I$(TPARTY_DIR)/kissfft \
  -I$(TPARTY_DIR)/ruy \
  -Imodels \
  -I.

CXXFLAGS ?= -std=c++17 -O3 -Wall -Wno-narrowing
CXXFLAGS += $(INCLUDES)

SRCS := hello_world_test.cc \
        models/hello_world_float_model_data.cc \
        models/hello_world_int8_model_data.cc

OBJS := $(SRCS:.cc=.o)

LIBTFLM := $(TFLM_BUILD_DIR)/libtflm.a

INSTALL_DIR    := $(EXTERNAL_BUILD_DIR)/project/tensorflow/hello_world/install
CP_TARGET_DIR  := $(INSTALL_DIR)

.PHONY: all clean

all: $(INSTALL_DIR)/hello_world.o

$(INSTALL_DIR)/hello_world.o: $(OBJS)
	@mkdir -p $(INSTALL_DIR)
	ld -r $^ -o $@
	

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(INSTALL_DIR)/hello_world.o
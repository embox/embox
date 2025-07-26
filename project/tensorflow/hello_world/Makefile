# Embox user Makefile for hello_world_test program

# Use Embox CC/CXX/CFLAGS/LDFLAGS by default if provided
CXX ?= g++
CC ?= gcc

# Relative to this Makefile's location
TFLM_BUILD_DIR ?= ../../../third-party/lib/tensorflow/build/tflite-micro
TFLM_ROOT      ?= ../../../third-party/lib/tensorflow/tflite-micro

# Additional third-party static deps (should exist after tflm build)
TPARTY_DIR ?= ../../../third-party/lib/tensorflow/third_party_static

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

TARGET := hello_world

LIBTFLM := $(TFLM_BUILD_DIR)/libtflm.a

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) $(LIBTFLM)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ -lm

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

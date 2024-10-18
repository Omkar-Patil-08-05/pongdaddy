CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17

RAYLIB_PATH ?= $(HOME)/raylib

SRC_DIR := src
OUT_DIR := outfiles
TARGET := $(OUT_DIR)/pong

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:.cpp=.o)

INCLUDE := -I$(RAYLIB_PATH)/src
LIB := -L$(RAYLIB_PATH)/src -lraylib -lssl -lcrypto

$(shell mkdir -p $(OUT_DIR))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(INCLUDE) $^ -o $@ $(LIB)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

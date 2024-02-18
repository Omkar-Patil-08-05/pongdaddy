CXX=g++
CXXFLAGS=-Wall -Wextra -std=c++17

RAYLIB=~/raylib

LIB=-L$(RAYLIB)/src -lraylib
INCLUDE=-I$(RAYLIB)/src

ORIGIN=src/main.cpp
TARGET=outfiles/pong

$(TARGET): $(ORIGIN)
	$(CXX) $(CFLAGS) $(INCLUDE) -o $@ $^  $(LIB)

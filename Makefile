CXX=g++
CXXFLAGS=-Wall -Wextra -std=c++17

RAYLIB=~/raylib
SRCS=~/dev/pongdaddy/src/main.cpp
OBJS = $(SRCS:.cpp=.o)
LIB=-L$(RAYLIB)/src -lraylib 
INCLUDE=-I$(RAYLIB)/src

ORIGIN=src/main.cpp
TARGET=outfiles/pong

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(INCLUDE) $^ -o $@ $(LIB)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


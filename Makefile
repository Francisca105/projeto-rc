# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17
CXXFLAGS += -fdiagnostics-color=always
CXXFLAGS += -Wall
CXXFLAGS += -Werror
CXXFLAGS += -Wextra
CXXFLAGS += -Wcast-align
CXXFLAGS += -Wconversion
CXXFLAGS += -Wfloat-equal
CXXFLAGS += -Wformat=2
CXXFLAGS += -Wnull-dereference
CXXFLAGS += -Wshadow
CXXFLAGS += -Wsign-conversion
CXXFLAGS += -Wswitch-default
CXXFLAGS += -Wswitch-enum
CXXFLAGS += -Wundef
CXXFLAGS += -Wunreachable-code
CXXFLAGS += -Wunused

# Files
SRC := src

CLIENT_SOURCES := $(wildcard $(SRC)/client/*.cpp)
CLIENT_HEADERS := $(wildcard $(SRC)/client/*.hpp)
CLIENT_OBJECTS := $(CLIENT_SOURCES:.cpp=.o)
CLIENT_EXEC := client

SERVER_SOURCES := $(wildcard $(SRC)/server/*.cpp)
SERVER_HEADERS := $(wildcard $(SRC)/server/*.hpp)
SERVER_OBJECTS := $(SERVER_SOURCES:.cpp=.o)
SERVER_EXEC := GS

COMMON_SOURCES := $(wildcard $(SRC)/common/*.cpp)
COMMON_HEADERS := $(wildcard $(SRC)/common/*.hpp)
COMMON_OBJECTS := $(COMMON_SOURCES:.cpp=.o)

SOURCES := $(CLIENT_SOURCES) $(SERVER_SOURCES) $(COMMON_SOURCES)
HEADERS := $(CLIENT_HEADERS) $(SERVER_HEADERS) $(COMMON_HEADERS)
OBJECTS := $(CLIENT_OBJECTS) $(SERVER_OBJECTS) $(COMMON_OBJECTS)
EXECS := $(CLIENT_EXEC) $(SERVER_EXEC)

# Commands
.PHONY: all clean fmt fmt-check

all: $(EXECS)

$(CLIENT_EXEC): $(CLIENT_OBJECTS) $(COMMON_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SERVER_EXEC): $(SERVER_OBJECTS) $(COMMON_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OBJECTS) $(EXECS) proj_50.zip

fmt: $(SOURCES) $(HEADERS)
	clang-format -i $^

submission:
	mkdir proj_50
	cp README.md proj_50/readme.txt
	cp -r src Makefile proj_50/
	zip -r proj_50.zip proj_50
	rm -r proj_50

CXX      := emcc
CXXFLAGS := -Wall -w -std=c++11
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
LIB_DIR  := $(BUILD)/lib
TARGET   := libjge.a
INCLUDE  := 
SRC_DIR  := src
HEADERS_DIR := include
EMCC_FLAGS := -s USE_LIBPNG=1

SOURCES := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

all: build $(LIB_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(EMCC_FLAGS) $(INCLUDE) -o $@ -c $<

$(LIB_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	emar rcsv $(LIB_DIR)/$(TARGET) $(OBJECTS)

.PHONY: all build clean debug release

debug: EMCC_FLAGS += -g4 --source-map-base -v -s ASSERTIONS=1  -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=1 -s DEMANGLE_SUPPORT=1
debug: all

release: EMCC_FLAGS += -O3
release: all

build:
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(OBJ_DIR)

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(LIB_DIR)/*
CXX      := emcc
CXXFLAGS := -Wall
LDFLAGS  := -std=c++11
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
LIB_DIR  := $(BUILD)/lib
TARGET   := libjge.a
INCLUDE  := 
SRC      := $(wildcard src/*.cpp)
EMCC_FLAGS := -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 --use-preload-plugins -O3
LIBS     := -L ../SDL2/lib -lSDL2 -lSDL2_image

OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(LIB_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(LIB_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	ar crf $(LIB_DIR)/$(TARGET) $(OBJECTS)

.PHONY: all build clean

build:
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(OBJ_DIR)

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(LIB_DIR)/*
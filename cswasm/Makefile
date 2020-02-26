CXX      := emcc
CXXFLAGS := -w #-pedantic-errors -Wall -Wextra
LDFLAGS  := -std=c++11
BUILD    := ./bin
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := index.html
INCLUDE := -I../JGE/include 
SRC      := $(wildcard src/*.cpp)
EMCC_FLAGS  := -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_LIBPNG=1 -s LEGACY_GL_EMULATION=1 -s ALLOW_MEMORY_GROWTH=1 --use-preload-plugins --preload-file ./bin/assets@/
LIBS 	 := -L../JGE/build/lib -ljge

OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(EMCC_FLAGS) $(INCLUDE) -o $@ -c $<

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(EMCC_FLAGS) $(LIBS) -o $(APP_DIR)/$(TARGET)

.PHONY: all build clean debug release

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: EMCC_FLAGS += -g4 --source-map-base -v -s ASSERTIONS=1 
debug: all

release: EMCC_FLAGS += -O3
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*

test:
	emcc test_main.cpp $(LDFLAGS) $(EMCC_FLAGS) $(CXXFLAGS) -g -o $(APP_DIR)/$(TARGET)
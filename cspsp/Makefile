CXX      := emcc
CXXFLAGS := -fdeclspec -w #-pedantic-errors -Wall -Wextra
LDFLAGS  := -std=c++11
BUILD    := ./bin
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := index.html
JGE_INCLUDE := -I../JGE/include 
SRC      := $(wildcard src/*.cpp)
EMCC_FLAGS  := -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 --use-preload-plugins -O3 --preload-file ./bin/assets@/
LIBS 	 := -L../JGE/build/lib -ljge

OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(JGE_INCLUDE) -o $@ -c $<

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(OBJECTS) $(CXXFLAGS) $(LIBS) $(JGE_INCLUDE) $(LDFLAGS) $(EMCC_FLAGS) -o $(APP_DIR)/$(TARGET)

.PHONY: all build clean debug release

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*

emcc:
	emcc main.cpp $(LDFLAGS) $(EMCC_FLAGS) $(JGE_INCLUDE) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET)
CC := gcc
DEBUG_FLAGS := -g -O0 -DDEBUG
RELEASE_FLAGS := -O2 -DNDEBUG

# По умолчанию debug, можно переопределить через make MODE=release
MODE ?= debug
BUILD_BASE_DIR := build
BUILD_DIR = $(BUILD_BASE_DIR)/$(MODE)
SRC_DIR := src
OBJ_DIR = $(BUILD_DIR)/obj
TARGET = $(BUILD_DIR)/program.exe

SOURCES := $(shell find $(SRC_DIR) -name "*.c")
OBJECTS := $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

ifeq ($(MODE), debug)
    CFLAGS = $(DEBUG_FLAGS)
else
    CFLAGS = $(RELEASE_FLAGS)
endif

all: build

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: build run clean

build: $(TARGET)

run: build
	./$(TARGET)

clean:
	rm -rf build

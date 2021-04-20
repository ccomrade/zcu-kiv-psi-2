#
# Makefile for GNU Make and GCC/Clang on Unix-like systems
#
# Usage:
#   make             # compile the source code
#   make all         # same as above
#   make clean       # remove all build files and the resulting executable
#   make CC=clang    # use Clang instead of GCC
#

CC := gcc

CFLAGS  := -Wall -Wextra -Wpedantic -std=c99 -O3 -DNDEBUG -D_POSIX_C_SOURCE=200112L -fvisibility=hidden -pthread
LDFLAGS := $(CFLAGS)

EXECUTABLE := http_server

SOURCE_DIR := src
BUILD_DIR  := build

SOURCE_FILES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECT_FILES := $(SOURCE_FILES:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPEND_FILES := $(OBJECT_FILES:%.o=%.d)

all: $(EXECUTABLE)

$(EXECUTABLE): $(BUILD_DIR) $(OBJECT_FILES)
	@echo "  LD      $@"
	@$(CC) -o $@ $(OBJECT_FILES) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@echo "  CC      $@"
	@$(CC) -c $(CFLAGS) -MMD -MP -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -r -f $(BUILD_DIR)/ $(EXECUTABLE)

.PHONY: all clean

# use the generated dependency files to properly track modifications of header files
-include $(DEPEND_FILES)

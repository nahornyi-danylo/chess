CC = gcc
CFLAGS = -Wall -O2 
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC_DIR = ../src
BUILD_SRC_DIR = .
OBJ_DIR = $(SRC_DIR)/objects
BUILD_OBJ_DIR = $(BUILD_SRC_DIR)/objects

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

BUILD_SRC = $(wildcard $(BUILD_SRC_DIR)/*.c)
BUILD_OBJ = $(patsubst $(BUILD_SRC_DIR)/%.c, $(BUILD_OBJ_DIR)/%.o, $(BUILD_SRC))

.SECONDARY: $(OBJ) $(BUILD_OBJ)
.SUFFIXES:

ifdef DEBUG
CFLAGS += -DDEBUG
endif

all: $(OBJ_DIR) $(BUILD_OBJ_DIR)

$(OBJ): | $(OBJ_DIR)

%: $(OBJ) $(BUILD_OBJ_DIR)/%.o
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_OBJ_DIR)/%.o: $(BUILD_SRC_DIR)/%.c | $(BUILD_OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

$(BUILD_OBJ_DIR):
	mkdir -p $@

clean:
	rm -f $(OBJ) $(BUILD_OBJ)

.PHONY: all clean

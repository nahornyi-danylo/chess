CC = gcc
CFLAGS = -Wall -O2 
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC_DIR = ../src
TEST_SRC_DIR = .
OBJ_DIR = $(SRC_DIR)/objects
TEST_OBJ_DIR = $(TEST_SRC_DIR)/objects

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

TEST_SRC = $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_OBJ = $(patsubst $(TEST_SRC_DIR)/%.c, $(TEST_OBJ_DIR)/%.o, $(TEST_SRC))

.SECONDARY: $(OBJ) $(TEST_OBJ)
.SUFFIXES:

ifdef DEBUG
CFLAGS += -DDEBUG
endif

all: $(OBJ_DIR) $(TEST_OBJ_DIR)

$(OBJ): | $(OBJ_DIR)

%: $(OBJ) $(TEST_OBJ_DIR)/%.o
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.c | $(TEST_OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

$(TEST_OBJ_DIR):
	mkdir -p $@

clean:
	rm -f $(OBJ) $(TEST_OBJ)

.PHONY: all clean

CC := gcc
CFLAGS := -g -std=c99 -Wall

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

EXE := $(BIN_DIR)/sri_memory_reader
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)


all: $(EXE)

$(EXE): $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

# read_cots_memory_app: sri_memory_reader.h sri_memory_reader.c component.h component.o address_iterator.h address_iterator.c
# 	$(CC) $(CFLAGS) -o $@ sri_memory_reader.c component.o address_iterator.c


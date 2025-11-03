CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude

# Dossiers
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# --- Librairie algorithmes ---
ALGO_SRC = $(SRC_DIR)/naif.c $(SRC_DIR)/satStructure.c
ALGO_OBJ = $(ALGO_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
ALGO_LIB = $(BUILD_DIR)/libalgos.a

# --- Librairie utilitaires ---
UTIL_SRC = $(SRC_DIR)/parser.c $(SRC_DIR)/timer.c
UTIL_OBJ = $(UTIL_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
UTIL_LIB = $(BUILD_DIR)/libutils.a

# --- Exécutables ---
EXE_SRC = $(SRC_DIR)/chrono.c $(SRC_DIR)/compare.c $(SRC_DIR)/resolve.c
EXE_OBJ = $(EXE_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
EXECS = $(BIN_DIR)/chrono $(BIN_DIR)/compare $(BIN_DIR)/resolve

# --- Interface Python ---
INTERFACE = interface.py

# Compile tout
all: $(ALGO_LIB) $(UTIL_LIB) $(EXECS)

# --- Librairie algos ---
$(ALGO_LIB): $(ALGO_OBJ)
	@mkdir -p $(BUILD_DIR)
	ar rcs $@ $^

# --- Librairie utilitaires ---
$(UTIL_LIB): $(UTIL_OBJ)
	@mkdir -p $(BUILD_DIR)
	ar rcs $@ $^

# --- Compilation exécutables ---
$(BIN_DIR)/%: $(BUILD_DIR)/%.o $(ALGO_LIB) $(UTIL_LIB)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -L$(BUILD_DIR) -lalgos -lutils

# --- Compilation générique ---
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

run: all
	python3 $(INTERFACE) $(ARGS)

.PHONY: all clean run

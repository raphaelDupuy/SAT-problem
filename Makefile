# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude

# Dossiers
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Fichiers sources
ALGO_SRC = $(SRC_DIR)/naif.c $(SRC_DIR)/satStructure.c
TIMER_SRC = $(SRC_DIR)/timerInterface.c $(SRC_DIR)/parser.c

# Fichiers objets
ALGO_OBJ = $(BUILD_DIR)/naif.o $(BUILD_DIR)/satStructure.o
TIMER_OBJ = $(TIMER_SRC:$(SRC_DIR)/%.c=build/%.o)

# Librairie et exécutable
LIB = $(BUILD_DIR)/libalgos.a
EXEC = $(BIN_DIR)/timerInterface
INTERFACE = interface.py

# ----------------------
# Règles principales
# ----------------------
all: $(LIB) $(EXEC)

# Création de la librairie statique
$(LIB): $(ALGO_OBJ)
	ar rcs $@ $^

# Compilation de l'exécutable timerInterface lié à la librairie
$(EXEC): $(TIMER_OBJ) $(LIB)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(TIMER_OBJ) -L$(BUILD_DIR) -lalgos

# Compilation générique des .c en .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Donner la commande à l'interface
run: $(INTERFACE)
	python3 $(INTERFACE) $(ARGS)

.PHONY: all clean run


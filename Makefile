CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude

# Dossiers
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
PICOSAT_DIR = $(SRC_DIR)/picosat/picosat-965

# --- Picosat ---
PICOSAT_LIB = $(PICOSAT_DIR)/libpicosat.a

# --- Librairie algorithmes ---
ALGO_SRC = $(SRC_DIR)/naif.c $(SRC_DIR)/satStructure.c $(SRC_DIR)/dpll.c $(SRC_DIR)/ppsz.c $(SRC_DIR)/schoning.c $(SRC_DIR)/schoningU.c $(SRC_DIR)/picosat_wrapper.c
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

# --- Graphiques ---
RESULTS_DIR = results
GRAPH_DIR = $(RESULTS_DIR)/graphs

# Compile tout
all: $(PICOSAT_LIB) $(ALGO_LIB) $(UTIL_LIB) $(EXECS)
	@echo "Build complete"

# --- Créer dossiers de résultats ---
$(RESULTS_DIR) $(GRAPH_DIR):
	@mkdir -p $@

# --- Compiler Picosat ---
$(PICOSAT_LIB):
	@echo "Building Picosat..."
	@cd $(PICOSAT_DIR) && ./configure.sh && make
	@echo "Picosat built successfully"

# --- Librairie algos ---
$(ALGO_LIB): $(ALGO_OBJ)
	@mkdir -p $(BUILD_DIR)
	ar rcs $@ $^
	@echo "Libalgos created"

# --- Librairie utilitaires ---
$(UTIL_LIB): $(UTIL_OBJ)
	@mkdir -p $(BUILD_DIR)
	ar rcs $@ $^
	@echo "Libutils created"

# --- Compilation picosat_wrapper ---
$(BUILD_DIR)/picosat_wrapper.o: $(SRC_DIR)/picosat_wrapper.c $(PICOSAT_LIB)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(PICOSAT_DIR) -c $< -o $@

# --- Compilation exécutables ---
$(BIN_DIR)/%: $(BUILD_DIR)/%.o $(ALGO_LIB) $(UTIL_LIB) $(PICOSAT_LIB)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(BUILD_DIR)/$*.o $(UTIL_LIB) $(ALGO_LIB) $(PICOSAT_LIB)
	@echo "$@ created"

# --- Compilation générique (sauf picosat_wrapper) ---
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Nettoyage ---
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@if [ -d $(PICOSAT_DIR) ]; then cd $(PICOSAT_DIR) && make clean; fi
	@echo "Clean done"

clean-all: clean
	@if [ -d $(PICOSAT_DIR) ]; then cd $(PICOSAT_DIR) && make distclean; fi
	@echo "Full clean done"

clean-results:
	rm -rf $(RESULTS_DIR)
	@echo "Results cleared"

# --- Commandes de benchmark ---
bench-sat: all $(RESULTS_DIR)
	@echo "Benchmarking SAT instances..."
	python3 $(INTERFACE) compare dpsc db/uf50-218
	@mkdir -p $(GRAPH_DIR)
	@mv benchmark_*.png benchmark_*.json $(GRAPH_DIR) 2>/dev/null || true

bench-unsat: all $(RESULTS_DIR)
	@echo "Benchmarking UNSAT instances..."
	python3 $(INTERFACE) compare dpsc db/uuf50-218
	@mkdir -p $(GRAPH_DIR)
	@mv benchmark_*.png benchmark_*.json $(GRAPH_DIR) 2>/dev/null || true

bench-all: bench-sat bench-unsat
	@echo "All benchmarks complete"
	@echo "Results saved in $(GRAPH_DIR)/"

# --- Commandes rapides ---
run: all
	python3 $(INTERFACE) $(ARGS)

help:
	@echo "Makefile commands:"
	@echo "  make              - Compile all"
	@echo "  make clean        - Clean build files"
	@echo "  make clean-all    - Clean everything including Picosat"
	@echo "  make clean-results- Clean benchmark results"
	@echo "  make run ARGS=... - Run with args"
	@echo "  make bench-sat    - Benchmark SAT instances"
	@echo "  make bench-unsat  - Benchmark UNSAT instances"
	@echo "  make bench-all    - Benchmark both"
	@echo ""
	@echo "Examples:"
	@echo "  make run ARGS='chrono c db/uf50-218/uf50-01.cnf'"
	@echo "  make run ARGS='compare cp db/uf50-218'"
	@echo "  make bench-all"

.PHONY: all clean clean-all clean-results bench-sat bench-unsat bench-all run help
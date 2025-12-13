# ============================================================
# Makefile for FFT Project
# AMSC-25-26 — fft-02-fft
# ============================================================

# -----------------------------
# Compilers
# -----------------------------

CXX      = mpic++
GXX      ?= g++
PYTHON   ?= python3

# -----------------------------
# Compilation flags
# -----------------------------
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O3
CPPFLAGS ?= -I./src -fopenmp
LDFLAGS  ?= -fopenmp
LDLIBS   ?=

# -----------------------------
# Directories
# -----------------------------
SRC_DIR  = src
UTIL_DIR = $(SRC_DIR)/utilities

# -----------------------------
# Dependency file
# -----------------------------
DEPEND = make.dep

# -----------------------------
# Executables
# -----------------------------
MAIN = main
GEN  = gen

# -----------------------------
# Source files
# -----------------------------
MAIN_SRCS = $(wildcard $(SRC_DIR)/*.cpp)
GEN_SRCS  = $(UTIL_DIR)/GenerateInput.cpp

# -----------------------------
# Object files
# -----------------------------
MAIN_OBJS = $(MAIN_SRCS:.cpp=.o)
GEN_OBJS  = $(GEN_SRCS:.cpp=.o)

# -----------------------------
# Phony targets
# -----------------------------
.PHONY:  all clean distclean generate \
        run-iterative run-recursive run-parallel run-all \
        plot plot-iterative plot-recursive plot-parallel

# ============================================================
# Default target
# ============================================================

all: $(DEPEND) $(MAIN) $(GEN)

# ============================================================
# Linking
# ============================================================

$(MAIN): $(MAIN_OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

# $(GEN): $(GEN_OBJS)
# 	$(GXX) $(CXXFLAGS) $^ -o $@
gen: $(GEN_OBJS)
	$(GXX) $(CXXFLAGS) $^ -o $@

# ============================================================
# Compilation rules
# ============================================================

# Rule for main source files (using MPI compiler)
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Rule for utility source files (using standard g++)
$(UTIL_DIR)/%.o: $(UTIL_DIR)/%.cpp
	$(GXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# ============================================================
# Dependency generation
# ============================================================

$(DEPEND): $(MAIN_SRCS) $(GEN_SRCS)
	@$(RM) $(DEPEND)
	@for file in $(MAIN_SRCS); do \
	  $(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $$file | \
	  sed 's|^|$(SRC_DIR)/|' >> $(DEPEND); \
	done
	@for file in $(GEN_SRCS); do \
	  $(GXX) $(CPPFLAGS) $(CXXFLAGS) -MM $$file | \
	  sed 's|^|$(UTIL_DIR)/|' >> $(DEPEND); \
	done

-include $(DEPEND)

# ============================================================
# Run targets
# ============================================================

# generate: $(GEN)
# 	./$(GEN)

generate: gen
	cd $(UTIL_DIR) && ../../gen
	find .. -name gen.txt -ls
	@if [ -f gen.txt ]; then \
		echo "Generated gen.txt in project root"; \
	fi

run-iterative: $(MAIN)
	./$(MAIN) 1 src/gen.txt

run-recursive: $(MAIN)
	./$(MAIN) 2 src/gen.txt

run-parallel: $(MAIN)
	./$(MAIN) 3 src/gen.txt

run-all: $(MAIN)
	./$(MAIN) 4 src/gen.txt

# ============================================================
# Plotting
# ============================================================

plot:
	$(PYTHON) $(SRC_DIR)/plot_results.py $(FILE)

plot-iterative:
	$(PYTHON) $(SRC_DIR)/plot_results.py output_Iterative.txt

plot-recursive:
	$(PYTHON) $(SRC_DIR)/plot_results.py output_Recursive.txt

plot-parallel:
	$(PYTHON) $(SRC_DIR)/plot_results.py output_Parallel.txt

# ============================================================
# Cleaning
# ============================================================

clean:
	$(RM) $(MAIN_OBJS) $(GEN_OBJS) $(DEPEND)

distclean: clean
	$(RM) $(MAIN) $(GEN)
	$(RM) output_*.txt *.png
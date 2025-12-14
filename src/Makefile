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
CPPFLAGS ?= -I.

# Detect OS to handle OpenMP support
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	# macOS (Apple Silicon) with Homebrew libomp
	# Make sure you installed:  brew install libomp
	OMP_CFLAGS  = -Xpreprocessor -fopenmp
	OMP_LDFLAGS = -Xpreprocessor -fopenmp -L/opt/homebrew/opt/libomp/lib
	OMP_LIBS    = -lomp
	CPPFLAGS   += -I/opt/homebrew/opt/libomp/include
else
	# Linux / other platforms with standard OpenMP support
	OMP_CFLAGS  = -fopenmp
	OMP_LDFLAGS = -fopenmp
	OMP_LIBS    =
endif

LDFLAGS  ?= $(OMP_LDFLAGS)
LDLIBS   ?= $(OMP_LIBS)

# -----------------------------
# Directories
# -----------------------------
SRC_DIR  = .
UTIL_DIR = utilities

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
MAIN_SRCS = main.cpp
GEN_SRCS  = $(UTIL_DIR)/GenerateInput.cpp

# -----------------------------
# Object files
# -----------------------------
MAIN_OBJS = main.o
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
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

# $(GEN): $(GEN_OBJS)
# 	$(GXX) $(CXXFLAGS) $^ -o $@
gen: $(GEN_OBJS)
	$(GXX) $(CXXFLAGS) $^ -o $@

# ============================================================
# Compilation rules
# ============================================================

# Rule for main source files (using MPI compiler)
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(OMP_CFLAGS) -c $< -o $@

# Rule for utility source files (using standard g++)
$(UTIL_DIR)/%.o: $(UTIL_DIR)/%.cpp
	$(GXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# ============================================================
# Dependency generation
# ============================================================

$(DEPEND): $(MAIN_SRCS) $(GEN_SRCS)
	@$(RM) $(DEPEND)
	@for file in $(MAIN_SRCS); do \
	  $(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $$file >> $(DEPEND); \
	done
	@for file in $(GEN_SRCS); do \
	  $(GXX) $(CPPFLAGS) $(CXXFLAGS) -MM $$file | \
	  sed 's|^|$(UTIL_DIR)/|' >> $(DEPEND); \
	done

-include $(DEPEND)

# ============================================================
# Run targets
# ============================================================

# Number of MPI processes (default: 4)
NP ?= 4

# generate: $(GEN)
# 	./$(GEN)

generate: gen
	./gen
	@if [ -f gen.txt ]; then \
		echo "Generated gen.txt in src folder"; \
	fi

run-iterative: $(MAIN)
	./$(MAIN) 1 gen.txt

run-recursive: $(MAIN)
	./$(MAIN) 2 gen.txt

run-parallel: $(MAIN)
	mpirun -np $(NP) ./$(MAIN) 3 gen.txt

run-all: $(MAIN)
	mpirun -np $(NP) ./$(MAIN) 4 gen.txt

# ============================================================
# Plotting
# ============================================================

plot:
	$(PYTHON) plot_results.py $(FILE)

plot-iterative:
	$(PYTHON) plot_results.py output_Iterative.txt

plot-recursive:
	$(PYTHON) plot_results.py output_Recursive.txt

plot-parallel:
	$(PYTHON) plot_results.py output_Parallel.txt

# ============================================================
# Cleaning
# ============================================================

clean:
	$(RM) $(MAIN_OBJS) $(GEN_OBJS) $(DEPEND)

distclean: clean
	$(RM) $(MAIN) $(GEN)
	$(RM) output_*.txt *.png
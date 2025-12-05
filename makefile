# Find all directories matching the pattern X.12, Y.12, 10.12, etc.
SUBDIRS := $(shell find . -maxdepth 1 -type d -regex '\./[0-9]+\.12')

# For each directory, check if main.c or main.rs exists
C_DIRS  := $(foreach d,$(SUBDIRS),$(if $(wildcard $(d)/main.c),$(d),))
R_DIRS  := $(foreach d,$(SUBDIRS),$(if $(wildcard $(d)/main.rs),$(d),))

# Output targets
C_TARGETS := $(C_DIRS:%=%/main)
R_TARGETS := $(R_DIRS:%=%/mainrs)

# Valgrind options
VALGRIND := valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes

all: $(C_TARGETS) $(R_TARGETS)

# --- Build rules -------------------------------------------------------------

# Build C programs → output: main
%/main: %/main.c
	$(CC) $< -O3 -o $@ -lm

# Build Rust programs → output: mainrs
%/mainrs: %/main.rs
	rustc $< -o $@

# --- Cleaning ---------------------------------------------------------------

clean:
	rm -f $(C_TARGETS) $(R_TARGETS)

# --- Running ----------------------------------------------------------------

run: all
	@echo "Running programs..."
	@for d in $(SUBDIRS); do \
		if [ -f "$$d/main" ] || [ -f "$$d/mainrs" ]; then \
			echo "==> Directory $$d"; \
			if [ -f "$$d/main" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "Running C program with valgrind and input.txt"; \
					$(VALGRIND) ./$$d/main $$d/input.txt; \
				else \
					echo "Running C program with valgrind (no input.txt)"; \
					$(VALGRIND) ./$$d/main; \
				fi; \
			fi; \
			if [ -f "$$d/mainrs" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "Running Rust program with input.txt"; \
					./$$d/mainrs $$d/input.txt; \
				else \
					echo "Running Rust program (no input.txt)"; \
					./$$d/mainrs; \
				fi; \
			fi; \
		fi; \
	done

# --- Running without Valgrind -----------------------------------------------

run-fast: all
	@echo "Running programs (without valgrind)..."
	@for d in $(SUBDIRS); do \
		if [ -f "$$d/main" ] || [ -f "$$d/mainrs" ]; then \
			echo "==> Directory $$d"; \
			if [ -f "$$d/main" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "Running C program"; \
					./$$d/main $$d/input.txt; \
				else \
					./$$d/main; \
				fi; \
			fi; \
			if [ -f "$$d/mainrs" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "Running Rust program"; \
					./$$d/mainrs $$d/input.txt; \
				else \
					./$$d/mainrs; \
				fi; \
			fi; \
		fi; \
	done

.PHONY: all clean run run-fast

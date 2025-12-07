# Find all directories matching the pattern X.12, Y.12, 10.12, etc.
SUBDIRS := $(shell find . -maxdepth 1 -type d -regex '\./[0-9]+\.12' | sort -V)

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
%/main: %/main.c nob.h
	$(CC) $< -O3 -o $@ -lm

# Build Rust programs → output: mainrs
%/mainrs: %/main.rs
	rustc $< -o $@

# --- Cleaning ---------------------------------------------------------------

clean:
	rm -f $(C_TARGETS) $(R_TARGETS)

# --- Running (with Valgrind, prettified) -------------------------------------

run: all
	@echo ""
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "🚀  Running programs (with Valgrind)"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo ""
	@for d in $(SUBDIRS); do \
		if [ -f "$$d/main" ] || [ -f "$$d/mainrs" ]; then \
			echo ""; \
			echo "📁 \033[1;34m$$d\033[0m"; \
			echo "────────────────────────────────────────────"; \
			if [ -f "$$d/main" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "🟢 \033[1;32mC + valgrind\033[0m (with input.txt)"; \
					$(VALGRIND) ./$$d/main $$d/input.txt; \
				else \
					echo "🟢 \033[1;32mC + valgrind\033[0m (no input.txt)"; \
					$(VALGRIND) ./$$d/main; \
				fi; \
			fi; \
			if [ -f "$$d/mainrs" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "🦀 \033[1;33mRust\033[0m (with input.txt)"; \
					./$$d/mainrs $$d/input.txt; \
				else \
					echo "🦀 \033[1;33mRust\033[0m (no input.txt)"; \
					./$$d/mainrs; \
				fi; \
			fi; \
		fi; \
	done
	@echo ""
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "✅  Done"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# --- Running without Valgrind (prettified) -----------------------------------

run-fast: all
	@echo ""
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "⚡  Running programs (FAST mode, no Valgrind)"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo ""
	@for d in $(SUBDIRS); do \
		if [ -f "$$d/main" ] || [ -f "$$d/mainrs" ]; then \
			echo ""; \
			echo "📁 \033[1;34m$$d\033[0m"; \
			echo "────────────────────────────────────────────"; \
			if [ -f "$$d/main" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "🟢 \033[1;32mC\033[0m (with input.txt)"; \
					./$$d/main $$d/input.txt; \
				else \
					echo "🟢 \033[1;32mC\033[0m (no input.txt)"; \
					./$$d/main; \
				fi; \
			fi; \
			if [ -f "$$d/mainrs" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "🦀 \033[1;33mRust\033[0m (with input.txt)"; \
					./$$d/mainrs $$d/input.txt; \
				else \
					echo "🦀 \033[1;33mRust\033[0m (no input.txt)"; \
					./$$d/mainrs; \
				fi; \
			fi; \
		fi; \
	done
	@echo ""
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "✅  Done"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

.PHONY: all clean run run-fast

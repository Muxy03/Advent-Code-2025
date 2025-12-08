# Find all directories matching the pattern X.12, Y.12, 10.12, etc.
SUBDIRS := $(shell find . -maxdepth 1 -type d -regex '\./[0-9]+\.12' | sort -V)

# For each directory, check if main.c, optimized.c, or main.rs exists
C_DIRS      := $(foreach d,$(SUBDIRS),$(if $(wildcard $(d)/main.c),$(d),))
OPT_DIRS    := $(foreach d,$(SUBDIRS),$(if $(wildcard $(d)/optimized.c),$(d),))
R_DIRS      := $(foreach d,$(SUBDIRS),$(if $(wildcard $(d)/main.rs),$(d),))

# Output targets
C_TARGETS   := $(C_DIRS:%=%/main)
OPT_TARGETS := $(OPT_DIRS:%=%/optimized)
R_TARGETS   := $(R_DIRS:%=%/mainrs)

# Valgrind options
VALGRIND := valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes

all: $(C_TARGETS) $(OPT_TARGETS) $(R_TARGETS)

# --- Build rules -------------------------------------------------------------

# Build C programs → output: main
%/main: %/main.c nob.h
	$(CC) $< -O3 -o $@ -lm

# Build optimized C programs → output: optimized
%/optimized: %/optimized.c nob.h
	$(CC) $< -O3 -o $@ -lm

# Build Rust programs → output: mainrs
%/mainrs: %/main.rs
	rustc $< -o $@

# --- Cleaning ---------------------------------------------------------------

clean:
	rm -f $(C_TARGETS) $(OPT_TARGETS) $(R_TARGETS)

# --- Running (with Valgrind, prettified) -------------------------------------

run: all
	@echo ""
	@echo "╔━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╗"
	@echo "║ 🚀  Running all programs (with Valgrind)     ║"
	@echo "╚━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╝"
	@echo ""
	@for d in $(SUBDIRS); do \
		if [ -f "$$d/main" ] || [ -f "$$d/optimized" ] || [ -f "$$d/mainrs" ]; then \
			echo ""; \
			echo "  📁 \033[1;34m$$d\033[0m"; \
			echo "  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"; \
			if [ -f "$$d/main" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "    🟢 \033[1;32mC (main)\033[0m"; \
					START=$$(date +%s%N); \
					$(VALGRIND) ./$$d/main $$d/input.txt 2>&1 | tee /tmp/val_out.txt | grep -E "^[Pp]art" | sed 's/^/      /'; \
					tail -6 /tmp/val_out.txt | sed 's/^/      /'; \
					END=$$(date +%s%N); \
					ELAPSED=$$(echo "scale=3; ($$END - $$START) / 1000000000" | bc); \
					echo "    ⏱  \033[1;35mTime: $${ELAPSED}s\033[0m"; \
					echo ""; \
				else \
					echo "    🟢 \033[1;32mC (main)\033[0m"; \
					START=$$(date +%s%N); \
					$(VALGRIND) ./$$d/main 2>&1 | tee /tmp/val_out.txt | grep -E "^[Pp]art" | sed 's/^/      /'; \
					tail -6 /tmp/val_out.txt | sed 's/^/      /'; \
					END=$$(date +%s%N); \
					ELAPSED=$$(echo "scale=3; ($$END - $$START) / 1000000000" | bc); \
					echo "    ⏱  \033[1;35mTime: $${ELAPSED}s\033[0m"; \
					echo ""; \
				fi; \
			fi; \
			if [ -f "$$d/optimized" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "    ⚡ \033[1;36mC (optimized)\033[0m"; \
					START=$$(date +%s%N); \
					$(VALGRIND) ./$$d/optimized $$d/input.txt 2>&1 | tee /tmp/val_out.txt | grep -E "^[Pp]art" | sed 's/^/      /'; \
					tail -6 /tmp/val_out.txt | sed 's/^/      /'; \
					END=$$(date +%s%N); \
					ELAPSED=$$(echo "scale=3; ($$END - $$START) / 1000000000" | bc); \
					echo "    ⏱  \033[1;35mTime: $${ELAPSED}s\033[0m"; \
					echo ""; \
				else \
					echo "    ⚡ \033[1;36mC (optimized)\033[0m"; \
					START=$$(date +%s%N); \
					$(VALGRIND) ./$$d/optimized 2>&1 | tee /tmp/val_out.txt | grep -E "^[Pp]art" | sed 's/^/      /'; \
					tail -6 /tmp/val_out.txt | sed 's/^/      /'; \
					END=$$(date +%s%N); \
					ELAPSED=$$(echo "scale=3; ($$END - $$START) / 1000000000" | bc); \
					echo "    ⏱  \033[1;35mTime: $${ELAPSED}s\033[0m"; \
					echo ""; \
				fi; \
			fi; \
			if [ -f "$$d/mainrs" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "    🦀 \033[1;33mRust\033[0m"; \
					START=$$(date +%s%N); \
					./$$d/mainrs $$d/input.txt | sed 's/^/      /'; \
					END=$$(date +%s%N); \
					ELAPSED=$$(echo "scale=3; ($$END - $$START) / 1000000000" | bc); \
					echo "    ⏱  \033[1;35mTime: $${ELAPSED}s\033[0m"; \
					echo ""; \
				else \
					echo "    🦀 \033[1;33mRust\033[0m"; \
					START=$$(date +%s%N); \
					./$$d/mainrs | sed 's/^/      /'; \
					END=$$(date +%s%N); \
					ELAPSED=$$(echo "scale=3; ($$END - $$START) / 1000000000" | bc); \
					echo "    ⏱  \033[1;35mTime: $${ELAPSED}s\033[0m"; \
					echo ""; \
				fi; \
			fi; \
		fi; \
	done
	@echo ""
	@echo "╔━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╗"
	@echo "║ ✅  All tests completed                      ║"
	@echo "╚━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╝"
	@echo ""
	@rm -f /tmp/val_out.txt
	@echo "🧹 Temporary files cleaned"
	@echo ""

# --- Running without Valgrind (prettified) -----------------------------------

run-fast: all
	@echo ""
	@echo "╔━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╗"
	@echo "║ ⚡ Running all programs (FAST mode)          ║"
	@echo "╚━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╝"
	@echo ""
	@for d in $(SUBDIRS); do \
		if [ -f "$$d/main" ] || [ -f "$$d/optimized" ] || [ -f "$$d/mainrs" ]; then \
			echo ""; \
			echo "  📁 \033[1;34m$$d\033[0m"; \
			echo "  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"; \
			if [ -f "$$d/main" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "    🟢 \033[1;32mC (main)\033[0m"; \
					START=$$(date +%s%N); \
					./$$d/main $$d/input.txt | sed 's/^/      /'; \
					END=$$(date +%s%N); \
					ELAPSED=$$(echo "scale=3; ($$END - $$START) / 1000000000" | bc); \
					echo "    ⏱  \033[1;35mTime: $${ELAPSED}s\033[0m"; \
					echo ""; \
				else \
					echo "    🟢 \033[1;32mC (main)\033[0m"; \
					START=$$(date +%s%N); \
					./$$d/main | sed 's/^/      /'; \
					END=$$(date +%s%N); \
					ELAPSED=$$(echo "scale=3; ($$END - $$START) / 1000000000" | bc); \
					echo "    ⏱  \033[1;35mTime: $${ELAPSED}s\033[0m"; \
					echo ""; \
				fi; \
			fi; \
			if [ -f "$$d/optimized" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "    ⚡ \033[1;36mC (optimized)\033[0m"; \
					START=$$(date +%s%N); \
					./$$d/optimized $$d/input.txt | sed 's/^/      /'; \
					END=$$(date +%s%N); \
					ELAPSED=$$(echo "scale=3; ($$END - $$START) / 1000000000" | bc); \
					echo "    ⏱  \033[1;35mTime: $${ELAPSED}s\033[0m"; \
					echo ""; \
				else \
					echo "    ⚡ \033[1;36mC (optimized)\033[0m"; \
					START=$$(date +%s%N); \
					./$$d/optimized | sed 's/^/      /'; \
					END=$$(date +%s%N); \
					ELAPSED=$$(echo "scale=3; ($$END - $$START) / 1000000000" | bc); \
					echo "    ⏱  \033[1;35mTime: $${ELAPSED}s\033[0m"; \
					echo ""; \
				fi; \
			fi; \
			if [ -f "$$d/mainrs" ]; then \
				if [ -f "$$d/input.txt" ]; then \
					echo "    🦀 \033[1;33mRust\033[0m"; \
					START=$$(date +%s%N); \
					./$$d/mainrs $$d/input.txt | sed 's/^/      /'; \
					END=$$(date +%s%N); \
					ELAPSED=$$(echo "scale=3; ($$END - $$START) / 1000000000" | bc); \
					echo "    ⏱  \033[1;35mTime: $${ELAPSED}s\033[0m"; \
					echo ""; \
				else \
					echo "    🦀 \033[1;33mRust\033[0m"; \
					START=$$(date +%s%N); \
					./$$d/mainrs | sed 's/^/      /'; \
					END=$$(date +%s%N); \
					ELAPSED=$$(echo "scale=3; ($$END - $$START) / 1000000000" | bc); \
					echo "    ⏱  \033[1;35mTime: $${ELAPSED}s\033[0m"; \
					echo ""; \
				fi; \
			fi; \
		fi; \
	done
	@echo ""
	@echo "╔━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╗"
	@echo "║ ✅  All tests completed                      ║"
	@echo "╚━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╝"
	@echo ""

.PHONY: all clean run run-fast

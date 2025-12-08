#include "../nob.h"

// Optimized Part 1: O(n) instead of O(n^2)
// Find max 2-digit number by tracking best pair
static inline ll Part1(const char *line, int len) {
	ll max_joltage = 0;
	
	// Track the largest digit seen so far
	int first_digit = line[0] - '0';
	
	for (int i = 1; i < len; ++i) {
		int second_digit = line[i] - '0';
		ll joltage = first_digit * 10 + second_digit;
		
		if (joltage > max_joltage) {
			max_joltage = joltage;
		}
		
		// Update first_digit if current is larger
		if (second_digit > first_digit) {
			first_digit = second_digit;
		}
	}
	
	return max_joltage;
}

// Optimized Part 2: Monge's algorithm (greedy stack)
static inline ll Part2(const char *line, int len) {
	if (len < 12) return 0;
	
	int digits_to_remove = len - 12;
	char stack[len];
	int top = 0;
	
	// Greedily build largest 12-digit number
	for (int i = 0; i < len; ++i) {
		char current = line[i];
		
		// Remove smaller digits to make room for larger ones
		while (top > 0 && digits_to_remove > 0 && stack[top - 1] < current) {
			top--;
			digits_to_remove--;
		}
		
		stack[top++] = current;
	}
	
	// Remove remaining digits if needed
	top -= digits_to_remove;
	
	// Convert to number using Horner's method
	ll joltage = 0;
	for (int i = 0; i < 12; ++i) {
		joltage = joltage * 10 + (stack[i] - '0');
	}
	
	return joltage;
}

int main(int argc, char *argv[]) {
	FILE *file = open_input_or_die(argc, argv);
	
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	ll part1 = 0, part2 = 0;
	
	while ((read = getline_or_die(&line, &len, file)) != -1) {
		// Remove trailing newline
		int line_len = (int)strlen(line);
		if (line_len > 0 && line[line_len - 1] == '\n') {
			line_len--;
		}
		
		if (line_len > 0) {
			part1 += Part1(line, line_len);
			part2 += Part2(line, line_len);
		}
	}
	
	printf("Part 1: %lld\n", part1);
	printf("Part 2: %lld\n", part2);
	
	free(line);
	fclose(file);
	
	return 0;
}

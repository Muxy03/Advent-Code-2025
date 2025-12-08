#include "../nob.h"

// Optimized: Check for double repeat (ABABAB...)
static inline bool is_double_repeat(const char *str, size_t len) {
	if (len % 2 != 0 || len == 0) return false;
	
	size_t half = len / 2;
	for (size_t i = 0; i < half; ++i) {
		if (str[i] != str[i + half]) return false;
	}
	return true;
}

// Optimized: Check for any repeating pattern
// Uses divisors check - much faster than checking all lengths
static inline bool is_multi_repeat(const char *str, size_t len) {
	if (len <= 1) return false;
	
	// Only check divisors of length
	for (size_t divisor = 1; divisor <= len / 2; ++divisor) {
		if (len % divisor != 0) continue;
		
		// Check if pattern repeats every 'divisor' chars
		bool match = true;
		for (size_t i = divisor; i < len; ++i) {
			if (str[i] != str[i % divisor]) {
				match = false;
				break;
			}
		}
		
		if (match) return true;
	}
	return false;
}

int main(int argc, char **argv) {
	FILE *input = open_input_or_die(argc, argv);
	
	char *line = NULL;
	size_t size = 0;
	ssize_t len = getline_or_die(&line, &size, input);
	fclose(input);
	
	if (len <= 0) {
		fprintf(stderr, "Error: could not read line from file.\n");
		free(line);
		return 1;
	}
	
	if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
	
	ll part1 = 0, part2 = 0;
	
	// Parse ranges directly without storing entire line
	const char *p = line;
	while (*p) {
		// Parse range: a-b
		ll a = strtoll(p, (char **)&p, 10);
		if (*p != '-') break;
		p++;
		
		ll b = strtoll(p, (char **)&p, 10);
		if (*p == ',') p++;
		
		// Process each number in range
		for (ll x = a; x <= b; ++x) {
			char buf[32];
			int buf_len = snprintf(buf, sizeof(buf), "%lld", x);
			
			// Check repeating patterns on the number string itself
			if (is_double_repeat(buf, (size_t)buf_len)) {
				part1 += x;
			}
			if (is_multi_repeat(buf, (size_t)buf_len)) {
				part2 += x;
			}
		}
	}
	
	printf("Part1: %lld\n", part1);
	printf("Part2: %lld\n", part2);
	
	free(line);
	return 0;
}

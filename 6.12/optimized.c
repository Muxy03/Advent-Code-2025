#include "../nob.h"

typedef struct {
	string *items;
	size_t count;
	size_t capacity;
} LINES;

// Optimized parse that handles leading spaces
static inline ll parse_ll_fast(const string s, size_t len) {
	ll v = 0;
	for (size_t i = 0; i < len; ++i) {
		uch c = (uch)s[i];
		if (c >= '0' && c <= '9') v = v * 10 + (c - '0');
	}
	return v;
}

int main(int argc, char **argv) {
	FILE *input = open_input_or_die(argc, argv);
	
	LINES lines = {0};
	ll part1 = 0, part2 = 0;
	string line = NULL;
	size_t size = 0, rows = 0, cols = 0;
	ssize_t read = 0;

	// Read input and find max column width
	while ((read = getline_or_die(&line, &size, input)) != -1) {
		size_t len = (size_t)read;
		if (len && line[len - 1] == '\n') line[--len] = '\0';
		
		string copy = (string)malloc(len + 1);
		memcpy(copy, line, len + 1);
		
		if (len > cols) cols = len;
		da_append(&lines, copy);
		rows++;
	}
	free(line);
	fclose(input);

	// Pad all lines to same length
	for (size_t i = 0; i < rows; ++i) {
		size_t cur = strlen(lines.items[i]);
		if (cur < cols) {
			lines.items[i] = (string)realloc(lines.items[i], cols + 1);
			memset(lines.items[i] + cur, ' ', cols - cur);
			lines.items[i][cols] = '\0';
		}
	}

	size_t op_row = rows - 1;
	bool *col_has = (bool *)calloc(cols, sizeof(bool));

	// Check which columns have data
	for (size_t c = 0; c < cols; ++c) {
		for (size_t r = 0; r < op_row; ++r) {
			if (lines.items[r][c] != ' ') {
				col_has[c] = true;
				break;
			}
		}
	}

	// Process column ranges
	for (size_t c = 0; c < cols;) {
		// Skip empty columns
		while (c < cols && !col_has[c]) ++c;
		if (c >= cols) break;

		size_t l = c;
		while (c < cols && col_has[c]) ++c;
		size_t r = c - 1;

		// Find operator in bottom row
		char op = '+';
		for (size_t cc = l; cc <= r; ++cc) {
			if (lines.items[op_row][cc] != ' ') {
				op = lines.items[op_row][cc];
				break;
			}
		}

		// PART 1: Process columns top to bottom
		ll acc1 = (op == '*') ? 1 : 0;
		bool used1 = false;

		for (size_t row = 0; row < op_row; ++row) {
			size_t len = r - l + 1;
			const string sub = lines.items[row] + l;
			
			// Find trimmed bounds
			size_t trim_start = 0;
			while (trim_start < len && isspace((uch)sub[trim_start])) trim_start++;
			
			size_t trim_end = len;
			while (trim_end > trim_start && isspace((uch)sub[trim_end - 1])) trim_end--;
			
			if (trim_start < trim_end) {
				ll v = parse_ll_fast(lines.items[row] + l + trim_start, trim_end - trim_start);
				used1 = true;
				if (op == '+') acc1 += v;
				else acc1 *= v;
			}
		}
		if (used1) part1 += acc1;

		// PART 2: Process columns right to left
		ll acc2 = (op == '*') ? 1 : 0;
		bool used2 = false;

		for (size_t cc = r + 1; cc-- > l;) {
			ll v = 0;
			bool found = false;

			// Parse column vertically with early termination
			for (size_t row = 0; row < op_row; ++row) {
				uch c = (uch)lines.items[row][cc];
				if (c >= '0' && c <= '9') {
					v = v * 10 + (c - '0');
					found = true;
				}
			}

			if (found) {
				used2 = true;
				if (op == '+') acc2 += v;
				else acc2 *= v;
			}
		}
		if (used2) part2 += acc2;
	}

	printf("Part 1: %llu\n", (ull)part1);
	printf("Part 2: %llu\n", (ull)part2);

	da_free_deep(&lines);
	free(col_has);

	return 0;
}

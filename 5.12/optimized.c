#include "../nob.h"

typedef struct {
	ll left;
	ll right;
} Range;

typedef struct {
	Range *items;
	size_t count;
	size_t capacity;
} Ranges;

typedef struct {
	ll *items;
	size_t count;
	size_t capacity;
} LLDA;

// Optimized comparators using inline
static inline int compare_ll(const void *a, const void *b) {
	ll x = *(const ll *)a;
	ll y = *(const ll *)b;
	return (x > y) - (x < y);
}

static inline int compare_ranges(const void *a, const void *b) {
	ll diff = ((const Range *)a)->left - ((const Range *)b)->left;
	return (diff > 0) - (diff < 0);
}

// Binary search with early termination
static inline bool is_in_range(const Range *arr, size_t len, ll id) {
	ssize_t l = 0, r = (ssize_t)len - 1;
	while (l <= r) {
		size_t mid = (size_t)(l + (r - l) / 2);
		if (arr[mid].left <= id && id <= arr[mid].right) {
			return true;
		}
		if (id < arr[mid].left) {
			r = (ssize_t)mid - 1;
		} else {
			l = (ssize_t)mid + 1;
		}
	}
	return false;
}

int main(int argc, char **argv) {
	FILE *input = open_input_or_die(argc, argv);

	char *line = NULL;
	size_t len = 0;
	bool is_elements = false;
	
	Ranges ranges = {0};
	LLDA elements = {0};

	while (getline(&line, &len, input) != -1) {
		// Remove trailing newline
		line[strcspn(line, "\r\n")] = '\0';
		
		// Skip empty lines and handle section transition
		if (strlen(line) == 0) {
			is_elements = true;
			continue;
		}

		if (!is_elements) {
			Range r;
			if (sscanf(line, "%lld-%lld", &r.left, &r.right) != 2) {
				fprintf(stderr, "Warning: malformed range '%s'.\n", line);
				exit(1);
			}
			da_append(&ranges, r);
		} else {
			da_append(&elements, atoll(line));
		}
	}

	fclose(input);
	free(line);

	// Sort both arrays
	qsort(elements.items, elements.count, sizeof(ll), compare_ll);
	qsort(ranges.items, ranges.count, sizeof(Range), compare_ranges);

	// Merge overlapping/adjacent ranges
	Ranges merged = {0};
	if (ranges.count > 0) {
		da_append(&merged, ranges.items[0]);
		
		for (size_t i = 1; i < ranges.count; ++i) {
			const Range *range = &ranges.items[i];
			Range *last = &merged.items[merged.count - 1];
			
			// Check if ranges overlap or are adjacent
			if (range->left <= last->right + 1) {
				// Merge: extend right boundary if needed
				if (range->right > last->right) {
					last->right = range->right;
				}
			} else {
				// No overlap, add new range
				da_append(&merged, *range);
			}
		}
	}

	// Count elements in ranges (Part 1)
	ll part1 = 0;
	for (size_t i = 0; i < elements.count; ++i) {
		if (is_in_range(merged.items, merged.count, elements.items[i])) {
			part1++;
		}
	}

	// Sum all range sizes (Part 2)
	ll part2 = 0;
	for (size_t i = 0; i < merged.count; ++i) {
		part2 += merged.items[i].right - merged.items[i].left + 1;
	}

	printf("part1: %lld\n", part1);
	printf("part2: %lld\n", part2);

	da_free(&ranges);
	da_free(&elements);
	da_free(&merged);

	return 0;
}

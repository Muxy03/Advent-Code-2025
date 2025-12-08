#include "../nob.h"

typedef struct {
	char **items;
	size_t count;
	size_t capacity;
} Matrix;

// Direction offsets for 8 neighbors
static const int dr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
static const int dc[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

// Fast neighbor counting with bounds checking
static inline int count_neighbors(const Matrix *m, int size, int i, int j) {
	int neighbors = 0;
	for (int k = 0; k < 8; ++k) {
		int nr = i + dr[k];
		int nc = j + dc[k];
		if (nr >= 0 && nr < size && nc >= 0 && nc < size && m->items[nr][nc] == '@') {
			neighbors++;
		}
	}
	return neighbors;
}

int main(int argc, char **argv) {
	FILE *input = open_input_or_die(argc, argv);
	
	Matrix m = {0};
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	while ((read = getline_or_die(&line, &len, input)) != -1) {
		line[strcspn(line, "\r\n")] = '\0';
		da_append(&m, strdup(line));
	}

	fclose(input);
	free(line);

	int size = (int)m.count;
	ll part1 = 0;
	ll part2 = 0;

	// PART 1: Count cells with < 4 neighbors
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			if (m.items[i][j] == '@' && count_neighbors(&m, size, i, j) < 4) {
				part1++;
			}
		}
	}

	// PART 2: Iteratively remove isolated cells
	while (1) {
		size_t round_removed = 0;

		// Use a temporary removal list to avoid modifying while iterating
		char *to_remove = (char *)malloc(size * size);
		memset(to_remove, 0, size * size);

		for (int i = 0; i < size; ++i) {
			for (int j = 0; j < size; ++j) {
				if (m.items[i][j] == '@' && count_neighbors(&m, size, i, j) < 4) {
					to_remove[i * size + j] = 1;
					round_removed++;
				}
			}
		}

		if (round_removed == 0) {
			free(to_remove);
			break;
		}

		// Apply removals
		for (int i = 0; i < size; ++i) {
			for (int j = 0; j < size; ++j) {
				if (to_remove[i * size + j]) {
					m.items[i][j] = '.';
				}
			}
		}

		free(to_remove);
		part2 += round_removed;
	}

	printf("Part1: %lld\n", part1);
	printf("Part2: %lld\n", part2);

	da_free_deep(&m);

	return 0;
}

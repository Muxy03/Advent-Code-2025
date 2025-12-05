#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef __cplusplus
#define NOB_DECLTYPE_CAST(T) (decltype(T))
#else
#define NOB_DECLTYPE_CAST(T)
#endif // __cplusplus

#define da_reserve(da, expected_capacity)                                                  \
    do {                                                                                   \
        if ((expected_capacity) > (da)->capacity) {                                        \
            if ((da)->capacity == 0) {                                                     \
                (da)->capacity = 256;                                                      \
            }                                                                              \
            while ((expected_capacity) > (da)->capacity) {                                 \
                (da)->capacity *= 2;                                                       \
            }                                                                              \
            (da)->items = NOB_DECLTYPE_CAST((da)->items)                                   \
                realloc((da)->items, (da)->capacity * sizeof(*(da)->items));               \
            assert((da)->items != NULL && "Out of memory");                                \
        }                                                                                  \
    } while (0)

#define da_append(da, item)                    \
    do {                                       \
        da_reserve((da), (da)->count + 1);     \
        (da)->items[(da)->count++] = (item);   \
    } while (0)

#define da_free(da)                            \
    do {                                       \
        for (size_t i = 0; i < (da).count; i++) free((da).items[i]); \
        free((da).items);                     \
    } while (0)


typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} Matrix;


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Error opening file");
        return 1;
    }

    Matrix m = {0};

	char *line = NULL;
  	size_t len = 0;
  	ssize_t read;			

   	while ((read = getline(&line, &len, input)) != -1) {
 		line[strcspn(line, "\r\n")] = '\0'; 
    	da_append(&m, strdup(line));
   	}

    int dr[8] = {-1,-1,-1, 0, 0, 1, 1, 1};
    int dc[8] = {-1, 0, 1,-1, 1,-1, 0, 1};

    size_t part1 = 0;
    size_t part2 = 0;
    int size = (int)m.count;

	// PART 1
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (m.items[i][j] != '@')
                continue;

            int neighbors = 0;

            for (int k = 0; k < 8; ++k) {
                int nr = i + dr[k];
                int nc = j + dc[k];

                if (nr >= 0 && nr < size && nc >= 0 && nc < size) {
                    if (m.items[nr][nc] == '@') {
                        neighbors++;
                    }
                }
            }

            if (neighbors < 4)
            	part1++;
        }
    }

    printf("Part1: %zu\n", part1);
    
	// PART 2
    while (1) {
        size_t round_removed = 0;
    
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
    
                if (m.items[i][j] != '@')
                    continue;
    
                int neighbors = 0;
    
                for (int k = 0; k < 8; ++k) {
                    int nr = i + dr[k];
                    int nc = j + dc[k];
    
                    if (nr >= 0 && nr < size &&
                        nc >= 0 && nc < size &&
                        m.items[nr][nc] == '@') {
                        neighbors++;
                    }
                }
    
                if (neighbors < 4) {
                    m.items[i][j] = '.';
                    round_removed++;
                }
            }
        }
    
        if (round_removed == 0)
            break;
    
        part2 += round_removed;
    }

    printf("Part2: %zu\n", part2);

    da_free(m);
    free(line);
    fclose(input);

    return 0;
}

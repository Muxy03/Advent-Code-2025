#include "../nob.h"

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} Matrix;


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

    int dr[8] = {-1,-1,-1, 0, 0, 1, 1, 1};
    int dc[8] = {-1, 0, 1,-1, 1,-1, 0, 1};

    ll part1 = 0;
    ll part2 = 0;
    int size = (int)m.count;

	// PART 1
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (m.items[i][j] != '@') continue;

            int neighbors = 0;

            for (int k = 0; k < 8; ++k) {
                int nr = i + dr[k];
                int nc = j + dc[k];

                if (nr >= 0 && nr < size && nc >= 0 && nc < size && m.items[nr][nc] == '@') neighbors++;
            }

            if (neighbors < 4) part1++;
        }
    }
 
	// PART 2
    while (1) {
        size_t round_removed = 0;
    
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
    
                if (m.items[i][j] != '@') continue;
    
                int neighbors = 0;
    
                for (int k = 0; k < 8; ++k) {
                    int nr = i + dr[k];
                    int nc = j + dc[k];
    
                    if (nr >= 0 && nr < size && nc >= 0 && nc < size && m.items[nr][nc] == '@') neighbors++;
                }
    
                if (neighbors < 4) {
                    m.items[i][j] = '.';
                    round_removed++;
                }
            }
        }
    
        if (round_removed == 0) break;
    
        part2 += round_removed;
    }

	printf("Part1: %lld\n", part1);
    printf("Part2: %lld\n", part2);

    da_free_deep(&m);
    free(line);
    fclose(input);

    return 0;
}

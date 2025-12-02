#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char **argv) {

	assert(argc == 2);

    char dir = '\0';
    int distance = -1;
    int state = 50;
    size_t part1 = 0, part2 = 0;
    char *line = NULL;
    size_t size = 0;
   	int tmp = -1;

	FILE *input = fopen(argv[1],"r");
	
    while (getline(&line, &size, input) != -1) {
		assert(line[0] == 'R' || line[0] == 'L');

    	dir = line[0];
    	distance = (int) strtol(line+1, NULL, 0);
    	
		tmp = dir == 'R' ? (100 - state) % 100 : state % 100;
        if (tmp == 0) tmp = 100;

		state = dir == 'R' ? (state + distance) % 100 : 
							 (state - distance) % 100; 
        if (state < 0) state += 100;
        
		if (distance >= tmp) part2 += 1 + (distance - tmp) / 100;

        if (state == 0) part1++;
    }

	free(line);
	fclose(input);
	
    printf("Part 1: %ld\n", part1);
    printf("Part 2: %ld\n", part2);
    return 0;
}

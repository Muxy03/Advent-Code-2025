#include "../nob.h"

int main(int argc, char **argv) {

	FILE *input = open_input_or_die(argc, argv);

    char dir = '\0';
    int distance = -1;
    int state = 50;
    ll part1 = 0, part2 = 0;
    string line = NULL;
    size_t size = 0;
   	int tmp = -1;

	
    while (getline_or_die(&line, &size, input) != -1) {
    
    	dir = line[0];
    	distance = (int) strtol(line+1, NULL, 0);
    	
		tmp = dir == 'R' ? (100 - state) % 100 : state % 100;
        if (tmp == 0) tmp = 100;

		state = dir == 'R' ? (state + distance) % 100 : (state - distance) % 100; 
        if (state < 0) state += 100;
        if (state == 0) part1++;
        
		if (distance >= tmp) part2 += 1 + (distance - tmp) / 100;
    }


    printf("Part 1: %lld\n", part1);
    printf("Part 2: %lld\n", part2);

	free(line);
	fclose(input);
	
    return 0;
}

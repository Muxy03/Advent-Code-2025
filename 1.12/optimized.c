#include "../nob.h"

int main(int argc, char **argv) {
	FILE *input = open_input_or_die(argc, argv);
	
	int state = 50;
	ll part1 = 0, part2 = 0;
	
	char *line = NULL;
	size_t size = 0;
	
	while (getline_or_die(&line, &size, input) != -1) {
		// Parse direction and distance
		char dir = line[0];
		int distance = (int)strtol(line + 1, NULL, 10);
		
		// Calculate distance to crossing point (state 0 or 100)
		int dist_to_crossing = (dir == 'R') ? (100 - state) : state;
		if (dist_to_crossing == 0) dist_to_crossing = 100;
		
		// Update state
		if (dir == 'R') {
			state = (state + distance) % 100;
		} else {
			state = (state - distance) % 100;
			if (state < 0) state += 100;
		}
		
		// Part 1: Count crossings of 0/100
		if (state == 0) part1++;
		
		// Part 2: Count all positions passing through 0/100
		if (distance >= dist_to_crossing) {
			part2 += 1 + (distance - dist_to_crossing) / 100;
		}
	}
	
	printf("Part 1: %lld\n", part1);
	printf("Part 2: %lld\n", part2);
	
	free(line);
	fclose(input);
	
	return 0;
}

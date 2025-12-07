#include "../nob.h"

ll Part1(const char *line) {
    int len = strlen(line) - 1;
    ll max_joltage = 0;

    for (int i = 0; i < len; i++) {
        for (int j = i + 1; j < len; j++) {
            ll joltage = (line[i] - '0') * 10 + (line[j] - '0');
            if (joltage > max_joltage) max_joltage = joltage;
        }
    }
    
    return max_joltage;
}

ll Part2(const char *line) {
    int len = strlen(line) - 1;
    if (len < 12) return 0;

    int digits_to_remove = len - 12;

    char stack[len+1];
    int top = 0;

    for (int i = 0; i < len; i++) {
    
        char current = line[i];

   		while (top > 0 && digits_to_remove > 0 && stack[top - 1] < current) {
            top--;
            digits_to_remove--;
        }

        stack[top++] = current;
    }

    top -= digits_to_remove;

    ll joltage = 0;

    for (int i = 0; i < 12; i++) {
        joltage = joltage * 10 + (stack[i] - '0');
    }

    return joltage;
}

int main(int argc, char *argv[]) {
    
    FILE *file = open_input_or_die(argc, argv);
    
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    ll part1 = 0;
    ll part2 = 0;

    while ((read = getline_or_die(&line, &len, file)) != -1) {
        if (strlen(line) > 0) {
            part1 += Part1(line);
            part2 += Part2(line);
        }
    }

    printf("Part 1: %lld\n", part1);
    printf("Part 2: %lld\n", part2);
    	
    free(line);
    fclose(file);
	
    return 0;
}

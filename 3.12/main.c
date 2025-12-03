#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long long Part1(const char *line) {
    int len = strlen(line) - 1;
    long long max_joltage = 0;

    for (int i = 0; i < len; i++) {
        for (int j = i + 1; j < len; j++) {
            long long joltage = (line[i] - '0') * 10 + (line[j] - '0');
            if (joltage > max_joltage) {
                max_joltage = joltage;
            }
        }
    }

    return max_joltage;
}

long long Part2(const char *line) {
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

    long long joltage = 0;
    for (int i = 0; i < 12; i++) {
        joltage = joltage * 10 + (stack[i] - '0');
    }

    return joltage;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    long long part1 = 0;
    long long part2 = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        if (strlen(line) > 0) {
            part1 += Part1(line);
            part2 += Part2(line);
        }
    }

    free(line);
    fclose(file);

    printf("Part 1: %lld\n", part1);
    printf("Part 2: %lld\n", part2);

    return 0;
}

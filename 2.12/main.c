#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool is_double_repeat(const char *s) {
    int n = strlen(s);
    if (n % 2 != 0) return false;
    int half = n / 2;
    return strncmp(s, s + half, half) == 0;
}

bool is_multi_repeat(const char *s) {
    int n = strlen(s);
    for (int len = 1; len <= n/2; len++) {
        if (n % len != 0) continue;
        int repeat = n / len;
        if (repeat < 2) continue;

        bool ok = true;
        for (int r = 1; r < repeat; r++) {
            if (strncmp(s, s + r*len, len) != 0) {
                ok = false;
                break;
            }
        }
        if (ok) return true;
    }
    return false;
}

int main(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "Error: missing input file.\n");
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Error opening input file");
        return 1;
    }

    char *line = NULL;
    size_t size = 0;

    if (getline(&line, &size, input) == -1) {
        fprintf(stderr, "Error: could not read line from file.\n");
        fclose(input);
        free(line);
        return 1;
    }

    fclose(input);

    long long part1 = 0;
    long long part2 = 0;

    char *token = strtok(line, ",");
    while (token) {
        long long a = 0, b = 0;

        if (sscanf(token, "%lld-%lld", &a, &b) != 2) {
            fprintf(stderr, "Warning: malformed range '%s' — skipped.\n", token);
            token = strtok(NULL, ",");
            continue;
        }

        if (a > b) {
            fprintf(stderr, "Warning: invalid range '%s' (start > end) — skipped.\n", token);
            token = strtok(NULL, ",");
            continue;
        }

        for (long long x = a; x <= b; x++) {
            char buf[32];
            sprintf(buf, "%lld", x);

            if (is_double_repeat(buf))
                part1 += x;

            if (is_multi_repeat(buf))
                part2 += x;
        }

        token = strtok(NULL, ",");
    }

    free(line);

    printf("Part1: %lld\n", part1);
    printf("Part2: %lld\n", part2);

    return 0;
}

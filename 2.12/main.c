#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool is_double_repeat(const char *s, int n) {
    if (n % 2 != 0) return false;
    int half = n / 2;
    return memcmp(s, s + half, half) == 0;
}

bool is_multi_repeat(const char *s, int n) {
    for (int len = 1; len <= n/2; len++) {
        if (n % len != 0) continue;
        
        // Check if all repeats match the first pattern
        bool ok = true;
        for (int i = len; i < n; i++) {
            if (s[i] != s[i % len]) {
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
        fprintf(stderr, "Error: missing input file.\nUsage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Error opening input file");
        return 1;
    }
    
    char *line = NULL;
    size_t size = 0;
    ssize_t len = getline(&line, &size, input);
    fclose(input);
    
    if (len == -1) {
        fprintf(stderr, "Error: could not read line from file.\n");
        free(line);
        return 1;
    }
    
    // Remove trailing newline if present
    if (len > 0 && line[len-1] == '\n')
        line[len-1] = '\0';
    
    long long part1 = 0, part2 = 0;
    char *saveptr = NULL;
    char *token = strtok_r(line, ",", &saveptr);
    
    while (token) {
        long long a = 0, b = 0;
        if (sscanf(token, "%lld-%lld", &a, &b) != 2) {
            fprintf(stderr, "Warning: malformed range '%s' — skipped.\n", token);
            token = strtok_r(NULL, ",", &saveptr);
            continue;
        }
        
        if (a > b) {
            fprintf(stderr, "Warning: invalid range '%s' (start > end) — skipped.\n", token);
            token = strtok_r(NULL, ",", &saveptr);
            continue;
        }
        
        char buf[32];
        for (long long x = a; x <= b; x++) {
            int buflen = sprintf(buf, "%lld", x);
            if (is_double_repeat(buf, buflen))
                part1 += x;
            if (is_multi_repeat(buf, buflen))
                part2 += x;
        }
        
        token = strtok_r(NULL, ",", &saveptr);
    }
    
    free(line);
    printf("Part1: %lld\n", part1);
    printf("Part2: %lld\n", part2);
    return 0;
}

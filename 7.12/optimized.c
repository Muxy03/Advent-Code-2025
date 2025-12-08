#include "../nob.h"

typedef struct {
    string *items;
    size_t count;
    size_t capacity;
} Lines;

typedef struct {
    size_t *items;
    size_t count;
    size_t capacity;
} Idxs;

typedef struct {
    ull *items;
    size_t count;
    size_t capacity;
} Big;

static inline void big_add_assign(Big *restrict a, const Big *restrict b) {
    if (!b->count) return;
    
    size_t maxlen = (a->count > b->count) ? a->count : b->count;
    da_reserve(a, maxlen + 1);
    
    __uint128_t carry = 0;
    size_t i = 0;
    
    for (; i < maxlen || carry; ++i) {
        ull av = (i < a->count) ? a->items[i] : 0;
        ull bv = (i < b->count) ? b->items[i] : 0;
        __uint128_t sum = (__uint128_t)av + bv + carry;
        ull out = (ull)sum;
        carry = (ull)(sum >> 64);
        
        if (i < a->count) {
            a->items[i] = out;
        } else {
            da_append(a, out);
        }
    }
    
    a->count = i;
    while (a->count && !a->items[a->count - 1]) a->count--;
}

int main(int argc, char **argv) {
    FILE *input = open_input_or_die(argc, argv);

    ll part1 = 0, part2 = 0;
    bool part2_big_needed = false;
    Big part2_big = {0};

    string line = NULL;
    size_t size = 0;
    ssize_t read = 0;

    Lines lines = {0};
    Idxs curr = {0}, next = {0};

    while ((read = getline_or_die(&line, &size, input)) != -1) {
        size_t len = strlen(line);
        if (len && line[len - 1] == '\n') line[--len] = '\0';
        da_append(&lines, strdup(line));
    }

    free(line);
    fclose(input);

    size_t rows = lines.count;
    size_t cols = strlen(lines.items[0]);
    size_t pos_S = cols / 2;

    // --- Part 1 ---
    bool *seen = (cols <= 8192) ? alloca(cols) : malloc(cols);
    
    if (rows > 1) da_append(&curr, pos_S);

    for (size_t r = 1; r < rows; ++r) {
        next.count = 0;
        string row_str = lines.items[r];

        for (size_t i = 0; i < curr.count; ++i) {
            size_t c = curr.items[i];
            if (c >= cols) continue;

            if (row_str[c] == '^') {
                part1++;
                if (c > 0) da_append(&next, c - 1);
                if (c + 1 < cols) da_append(&next, c + 1);
            } else {
                da_append(&next, c);
            }
        }

        memset(seen, 0, cols);
        size_t new_count = 0;

        for (size_t i = 0; i < next.count; ++i) {
            size_t c = next.items[i];
            if (!seen[c]) {
                seen[c] = true;
                next.items[new_count++] = c;
            }
        }
        next.count = new_count;
        da_swap(curr, next);
    }

    if (cols > 8192) free(seen);
    da_free(&curr);
    da_free(&next);

    // --- Part 2 ---
    Big *currB, *nextB;
    bool heap_alloc = cols > 1024;
    
    if (heap_alloc) {
        currB = malloc(cols * sizeof(Big));
        nextB = malloc(cols * sizeof(Big));
    } else {
        currB = alloca(cols * sizeof(Big));
        nextB = alloca(cols * sizeof(Big));
    }

    for (size_t i = 0; i < cols; ++i) {
        currB[i] = (Big){0};
        nextB[i] = (Big){0};
    }

    if (rows > 1) da_append(&currB[pos_S], 1ULL);

    for (size_t r = 1; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) nextB[c].count = 0;
        
        string row_str = lines.items[r];

        for (size_t c = 0; c < cols; ++c) {
            if (currB[c].count == 0) continue;
            
            if (row_str[c] == '^') {
                if (c > 0) big_add_assign(&nextB[c - 1], &currB[c]);
                if (c + 1 < cols) big_add_assign(&nextB[c + 1], &currB[c]);
            } else {
                big_add_assign(&nextB[c], &currB[c]);
            }
        }

        for (size_t i = 0; i < cols; ++i) {
            da_swap(currB[i], nextB[i]);
        }
    }

    // Accumulate results efficiently
    for (size_t c = 0; c < cols; ++c) {
        if (currB[c].count == 0) continue;

        if (!part2_big_needed) {
            if (currB[c].count > 1 || currB[c].items[0] > (ULLONG_MAX - part2)) {
                part2_big_needed = true;
                if (part2) {
                    Big tmp = {0};
                    da_append(&tmp, (ull)part2);
                    big_add_assign(&part2_big, &tmp);
                    da_free(&tmp);
                }
                part2 = 0;
            }
        }

        if (part2_big_needed) {
            big_add_assign(&part2_big, &currB[c]);
        } else {
            part2 += (ll)currB[c].items[0];
        }
    }

    printf("part1: %lld\n", part1);
    if (part2_big_needed) {
        if (part2_big.count) {
            printf("part2: %llu\n", part2_big.items[0]);
        } else {
            printf("part2: 0\n");
        }
    } else {
        printf("part2: %lld\n", part2);
    }

    for (size_t i = 0; i < cols; ++i) {
        da_free(&currB[i]);
        da_free(&nextB[i]);
    }

    if (heap_alloc) {
        free(currB);
        free(nextB);
    }
    
    da_free_deep(&lines);
    da_free(&part2_big);

    return 0;
}
